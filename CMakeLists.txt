cmake_minimum_required(VERSION 3.16)
project(CarbonUI_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
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

find_package(Qt5 CONFIG REQUIRED COMPONENTS Core Gui Widgets)
set(QT_VERSION_MAJOR 5)

qt_standard_project_setup()


set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
)

# Put Qt .ui files in a separate variable so IDEs show them under a distinct category
set(PROJECT_UI
    MainWindow.ui
)

# Put header files in a separate variable so IDEs show them under a distinct category
set(PROJECT_HEADERS
    MainWindow.h
)

set(RESOURCES_RCC
    Resource.qrc
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_UI} ${RESOURCES_RCC})

set_target_properties(${PROJECT_NAME}    PROPERTIES        WIN32_EXECUTABLE TRUE)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
)

# Ensure source files are interpreted as UTF-8 by the compiler
# - MSVC: use /utf-8
# - GCC/Clang: use -finput-charset and -fexec-charset
if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# Option to control whether CarbonUI is built/used as shared library
option(BUILD_CARBONUI_SHARED "Build CarbonUI as shared library" OFF)

# Set BUILD_SHARED_LIBS according to BUILD_CARBONUI_SHARED so CPM/FetchContent will build CarbonUI appropriately
# Note: this sets the cache variable which may affect other projects added after this point
if(BUILD_CARBONUI_SHARED)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
else()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
endif()

# ########################  3rd party dependencies  #########################
include(FetchContent)
include(cmake/CPM.cmake)

CPMAddPackage(
    NAME CarbonUI
    GITHUB_REPOSITORY FOHEART/CarbonUI
    GIT_TAG main)

# Link the CarbonUI target provided by CPM to the project executable
target_link_libraries(${PROJECT_NAME} PRIVATE CarbonUI)

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

# On Windows, try to run windeployqt to automatically copy required Qt DLLs into the target folder
if (WIN32)
    # Attempt to find windeployqt on PATH or in common Qt installation locations
    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt.exe windeployqt HINTS $ENV{QTDIR} $ENV{QT_DIR} PATH)
    if (NOT WINDEPLOYQT_EXECUTABLE)
        # Also try near the Qt5_DIR if defined
        if (DEFINED Qt5_DIR)
            get_filename_component(_qt_dir "${Qt5_DIR}" DIRECTORY)
            get_filename_component(_qt_bin "${_qt_dir}/bin" ABSOLUTE)
            find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt.exe windeployqt HINTS "${_qt_bin}")
        endif()
    endif()

    if (WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Running windeployqt to deploy Qt runtime DLLs"
            # Use the built executable path as the target for deployment
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Auto-deploying Qt DLLs with windeployqt"
        )
    else()
        message(WARNING "windeployqt not found. Qt runtime DLLs will not be auto-copied. Install Qt bin directory on PATH or set QTDIR/Qt5_DIR.")
    endif()
endif()

