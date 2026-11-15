cmake_minimum_required(VERSION 3.16)
project(CarbonUI_app LANGUAGES CXX)

# GoogleTest 1.14 起要求 C++14；Qt 5.15 完全兼容
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/qt.cmake)

# --- Qt5 discovery (cross-platform) ---
# Support common environment variables used by local installs and CI actions.
# - Qt5_DIR: path containing Qt5Config.cmake
# - QT_DIR / QTDIR: Qt prefix (will be appended to CMAKE_PREFIX_PATH)
if (NOT DEFINED Qt5_DIR)
    if (DEFINED ENV{Qt5_DIR})
        set(Qt5_DIR "$ENV{Qt5_DIR}")
    endif()
endif()

foreach(_qt_prefix_var QT_DIR QTDIR)
    if (DEFINED ENV{${_qt_prefix_var}})
        list(PREPEND CMAKE_PREFIX_PATH "$ENV{${_qt_prefix_var}}")
    endif()
endforeach()

find_package(Qt5 CONFIG REQUIRED COMPONENTS Core Gui Widgets Test)
set(QT_VERSION_MAJOR 5)

qt_standard_project_setup()


# ########################  3rd party dependencies  #########################
# CarbonUI 以 git submodule 形式引入（third_party/CarbonUI），直接 add_subdirectory。
set(BUILD_CARBONUI_SHARED OFF CACHE BOOL "Build CarbonUI as shared library" FORCE)
add_subdirectory(third_party/CarbonUI)

# ########################  应用库（可测核心）  ##############################
# 主窗口 + 纯逻辑模块编成静态库，供可执行文件与测试目标共享同一套编译选项
# （TDD 约定：测试与实现同仓同构建，共享标准/编码/运行时库设置）。
set(APPLIB_SOURCES
    MainWindow.cpp
    core/AppFont.cpp
)

set(APPLIB_HEADERS
    MainWindow.h
    core/AppFont.h
)

set(APPLIB_UI
    MainWindow.ui
)

set(APPLIB_RCC
    Resource.qrc
)

add_library(applib STATIC ${APPLIB_SOURCES} ${APPLIB_HEADERS} ${APPLIB_UI} ${APPLIB_RCC})
target_include_directories(applib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/core"
        "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(applib
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        CarbonUI
)

# ########################  主程序  ##############################
set(PROJECT_SOURCES
    main.cpp
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME}    PROPERTIES        WIN32_EXECUTABLE TRUE)

# Set final build output directory to "output" folder at project root
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/output")
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        applib
        CarbonUI
)

# If CarbonUI is a shared/module library, copy its runtime file to the executable folder after build
if (TARGET CarbonUI)
    get_target_property(_CARBONUI_TYPE CarbonUI TYPE)
    if (_CARBONUI_TYPE STREQUAL "SHARED_LIBRARY" OR _CARBONUI_TYPE STREQUAL "MODULE_LIBRARY")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying CarbonUI runtime to target folder"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:CarbonUI>"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_NAME:CarbonUI>"
            COMMENT "Copying CarbonUI runtime to runtime directory"
        )
    endif()
endif()

# After build, copy the 'fonts' directory from the source tree to the target runtime directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying fonts directory to runtime folder"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/fonts"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts"
    COMMENT "Copying fonts to runtime directory"
)

# Run windeployqt (if available) to deploy Qt runtime DLLs for the executable
# Function `run_windeployqt` is defined in cmake/qt.cmake and is a no-op on non-Windows platforms.
run_windeployqt(${PROJECT_NAME})

# Ensure source files are interpreted as UTF-8 by the compiler
# - MSVC: use /utf-8
# - GCC/Clang: use -finput-charset and -fexec-charset
if (MSVC)
  add_compile_options(/utf-8)
else()
  add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# ---------------- CPack packaging ----------------
# Use a separate cmake/cpack.cmake to setup packaging cmd: cpack -G ZIP
include(cmake/cpack.cmake)

# Allow callers to override CPACK_PACKAGE_VERSION and CPACK_PACKAGE_CONTACT before calling
setup_cpack_for_target(${PROJECT_NAME} "${OUTPUT_DIR}")

# ########################  测试（GoogleTest + QtTest）  ######################
enable_testing()

option(USE_SYSTEM_GTEST "Use installed GoogleTest" OFF)
set(GTEST_REPO "https://github.com/google/googletest.git" CACHE STRING "")
set(GTEST_TAG  "v1.14.0" CACHE STRING "")

if(USE_SYSTEM_GTEST)
    find_package(GTest 1.11 REQUIRED)
    set(GTEST_TARGET GTest::gtest)
else()
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)   # MSVC：与主工程同为 /MD
    set(INSTALL_GTEST OFF        CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF          CACHE BOOL "" FORCE)

    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY "${GTEST_REPO}"
        GIT_TAG        "${GTEST_TAG}"
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE)
    FetchContent_MakeAvailable(googletest)
    set(GTEST_TARGET GTest::gtest)
endif()

include(GoogleTest)

# 单元测试：纯逻辑层（AppFont），GoogleTest + 自定义 main（先 InitGoogleTest 再建 QApplication）
add_executable(app_unit_tests tests/unit/appfont_test.cpp)
target_link_libraries(app_unit_tests PRIVATE applib ${GTEST_TARGET})
target_compile_definitions(app_unit_tests PRIVATE
    CARBONUI_APP_TEST_FONT="${CMAKE_SOURCE_DIR}/fonts/HarmonyOS_Sans_SC_Bold.ttf")
gtest_discover_tests(app_unit_tests
    TEST_PREFIX "Unit."
    PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 120
    DISCOVERY_MODE PRE_TEST)

# 界面测试：MainWindow 冒烟（QtTest），离屏运行、只断言行为不做像素比对
add_executable(app_ui_tests tests/ui/mainwindow_test.cpp)
target_link_libraries(app_ui_tests PRIVATE applib Qt${QT_VERSION_MAJOR}::Test)
add_test(NAME Ui.MainWindow COMMAND app_ui_tests)
set_tests_properties(Ui.MainWindow PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 120)

# ---------------- Doxygen 文档（可选目标：cmake --build . --target docs） ----------------
find_program(DOXYGEN_EXECUTABLE doxygen
    HINTS "D:/autotools/doxygen-1.11.0"
    "$ENV{DOXYGEN_HOME}")
if(DOXYGEN_EXECUTABLE)
    message(STATUS "doxygen found: ${DOXYGEN_EXECUTABLE}")
    add_custom_target(docs
        COMMAND "${DOXYGEN_EXECUTABLE}" Doxyfile
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
else()
    message(STATUS "doxygen not found, 'docs' target disabled (set DOXYGEN_HOME or install Doxygen)")
endif()
