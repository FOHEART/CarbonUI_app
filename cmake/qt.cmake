if(QT_VERSION VERSION_LESS 6.3)
    macro(qt_standard_project_setup)
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTOUIC ON)
        set(CMAKE_AUTORCC ON)
    endmacro()
endif()

if(QT_VERSION VERSION_LESS 6.0)
    macro(qt_add_executable name)
         if(ANDROID)
            add_library(name SHARED ${ARGN})
        else()
            add_executable(${ARGV})
        endif()
    endmacro()

    # Qt5 doesn't provide qt_add_resources(); enable AUTORCC instead.
    macro(qt_add_resources out_var)
        # No explicit resource target needed in Qt5 when CMAKE_AUTORCC is ON and .qrc is part of sources.
        set(${out_var} ${ARGN})
    endmacro()
endif()
