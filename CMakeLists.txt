cmake_minimum_required(VERSION 3.16)
project(CarbonUI_app LANGUAGES CXX)

# ----------------------------------------------------------------------------
# 生成物统一收敛到 out/（见技能 references/build-scripts/CMake构建输出目录规范）：
#   out/build           Windows 构建树；out/build-linux  Linux/WSL 构建树（按系统隔离）
#   out/build*/output/<Config>  可直接运行的目录（exe + Qt DLL + Resources，按配置隔离）
#   out/install         部署树；out/package 分发包；out/docs 文档；out/tidy 静态检查报告
# ----------------------------------------------------------------------------
if(WIN32)
  set(PROJECT_BUILD_ROOT "${CMAKE_SOURCE_DIR}/out/build")
else()
  set(PROJECT_BUILD_ROOT "${CMAKE_SOURCE_DIR}/out/build-linux")
endif()
set(OUTPUT_DIR "${PROJECT_BUILD_ROOT}/output/$<CONFIG>")

# GoogleTest 1.14 起要求 C++14；Qt 5.15 完全兼容
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 导出编译数据库供 clang-tidy / cpp-linter 使用（VS 生成器忽略该设置，
# 代码检查统一走 scripts\lint.bat，其内部用 Ninja 生成 out/build/lint）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 源码统一 UTF-8（含中文注释/字面量）。目录属性只对"其后创建"的目标生效，
# 因此必须放在任何 add_library/add_executable 之前，否则 MSVC 会报 C4566。
if (MSVC)
  add_compile_options(/utf-8)
else()
  add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

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
# CarbonUI 以子模块形式引入（third_party/CarbonUI），直接 add_subdirectory。
# 用 option() 而非 CACHE FORCE：CI 矩阵经 -DBUILD_CARBONUI_SHARED=ON/OFF 传值时须被尊重。
option(BUILD_CARBONUI_SHARED "Build CarbonUI as shared library" OFF)
add_subdirectory(third_party/CarbonUI)

# ########################  应用库（可测核心）  ##############################
# 主窗口 + 纯逻辑模块编成静态库，供可执行文件与测试目标共享同一套编译选项
# （TDD 约定：测试与实现同仓同构建，共享标准/编码/运行时库设置）。
set(APPLIB_SOURCES
    src/MainWindow.cpp
    src/GalleryPage.cpp
    src/ButtonPage.cpp
    src/SelectionPage.cpp
    src/InputPage.cpp
    src/ListPage.cpp
    src/TextOverflowPage.cpp
    src/DialogPage.cpp
    src/ScrollPage.cpp
    src/AppFont.cpp
)

set(APPLIB_HEADERS
    inc/MainWindow.h
    inc/GalleryPage.h
    inc/ButtonPage.h
    inc/SelectionPage.h
    inc/InputPage.h
    inc/ListPage.h
    inc/TextOverflowPage.h
    inc/DialogPage.h
    inc/ScrollPage.h
    inc/AppFont.h
)

set(APPLIB_UI
    ui/MainWindow.ui
)

set(APPLIB_RCC
    Resource.qrc
)

add_library(applib STATIC ${APPLIB_SOURCES} ${APPLIB_HEADERS} ${APPLIB_UI} ${APPLIB_RCC})
# .ui 文件集中在 ui/ 目录，AUTOUIC 默认只按源文件所在目录搜索，需显式指定
set_target_properties(applib PROPERTIES AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")
# ui_MainWindow.h 由 uic 生成：.ui 中的中文被转成八进制转义，后跟数字（如"设置2"
# 的 2）会触发 MSVC C4125（八进制转义被十进制数字终止）。这是生成代码的固有形态，
# 仅对包含该头文件的编译单元精确豁免，其余本工程代码保持 /W4 /WX 严格告警。
if(MSVC)
    set_source_files_properties(src/MainWindow.cpp PROPERTIES COMPILE_OPTIONS /wd4125)
endif()
target_include_directories(applib
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
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
    src/main.cpp
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME}    PROPERTIES        WIN32_EXECUTABLE TRUE)

# 最终产物落到 out/build*/output/<Config>，不再写仓库根 output/
# （$<CONFIG> 已按配置隔离，多配置生成器不会再自动追加一层配置子目录）
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
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

# After build, copy the whole 'Resources' directory (fonts + styles QSS) to the runtime directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying Resources directory to runtime folder"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources"
    COMMENT "Copying Resources to runtime directory"
)

# Run windeployqt (if available) to deploy Qt runtime DLLs for the executable
# Function `run_windeployqt` is defined in cmake/qt.cmake and is a no-op on non-Windows platforms.
run_windeployqt(${PROJECT_NAME})

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
    CARBONUI_APP_TEST_FONT="${CMAKE_SOURCE_DIR}/Resources/fonts/HarmonyOS_Sans_SC_Bold.ttf")
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

# 截图基线测试：整窗截取每个 Tab 页与「测试」对话框，与
# tests/visual/baseline/<平台>/*.png 比对，拦截显示风格回归。
# 复用 CarbonUI 子库的比对工具（只读 include，不链接其测试目标）。
add_executable(app_visual_tests
    tests/visual/main.cpp
    tests/visual/app_screenshot_test.cpp)
target_link_libraries(app_visual_tests PRIVATE applib ${GTEST_TARGET} Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(app_visual_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/CarbonUI/tests/support")

# 基线按平台分目录：字体引擎不同（DirectWrite / FreeType），跨平台共用基线必然假红。
if(WIN32)
    set(APP_SCREENSHOT_PLATFORM "windows")
    set(APP_VISUAL_QPA windows)
elseif(APPLE)
    set(APP_SCREENSHOT_PLATFORM "macos")
    set(APP_VISUAL_QPA offscreen)
else()
    set(APP_SCREENSHOT_PLATFORM "linux")
    set(APP_VISUAL_QPA offscreen)
endif()
target_compile_definitions(app_visual_tests PRIVATE
    CARBONUI_SCREENSHOT_BASELINE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/visual/baseline"
    CARBONUI_SCREENSHOT_PLATFORM="${APP_SCREENSHOT_PLATFORM}"
    CARBONUI_APP_VISUAL_FONT="${CMAKE_CURRENT_SOURCE_DIR}/Resources/fonts/HarmonyOS_Sans_SC_Regular.ttf"
    CARBONUI_APP_VISUAL_QSS="${CMAKE_CURRENT_SOURCE_DIR}/Resources/styles/app.qss")

# 视觉套件本地默认跳过（见 tests/visual/app_screenshot_test.cpp 的开关说明）；
# gtest_discover_tests 的 PRE_TEST 发现只列用例，不会真正比对。
gtest_discover_tests(app_visual_tests
    TEST_PREFIX "Visual."
    PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=${APP_VISUAL_QPA}" TIMEOUT 180
    DISCOVERY_MODE PRE_TEST)

# ########################  编译告警等级（仅本工程目标）  ######################
# MSVC: /W4 + /WX（告警当错误）；GCC/Clang: -Wall -Wextra -Werror。
# 只加在 applib / 主程序 / 两个测试目标上，不影响 third_party/CarbonUI 与
# FetchContent 拉取的 googletest。
if(MSVC)
    set(PROJECT_WARNING_FLAGS /W4 /WX)
else()
    set(PROJECT_WARNING_FLAGS -Wall -Wextra -Werror)
endif()
foreach(_project_target applib ${PROJECT_NAME} app_unit_tests app_ui_tests app_visual_tests)
    target_compile_options(${_project_target} PRIVATE ${PROJECT_WARNING_FLAGS})
endforeach()

# ---------------- Doxygen 文档（可选目标：cmake --build . --target docs） ----------------
find_program(DOXYGEN_EXECUTABLE doxygen
    HINTS "D:/autotools/doxygen-1.11.0"
    "$ENV{DOXYGEN_HOME}")
if(DOXYGEN_EXECUTABLE)
    message(STATUS "doxygen found: ${DOXYGEN_EXECUTABLE}")
    add_custom_target(docs
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_SOURCE_DIR}/out/doxygen"
        COMMAND "${DOXYGEN_EXECUTABLE}" Doxyfile
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
else()
    message(STATUS "doxygen not found, 'docs' target disabled (set DOXYGEN_HOME or install Doxygen)")
endif()
