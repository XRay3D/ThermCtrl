#/*******************************************************************************
#* Author    :  Damir Bakiev                                                    *
#* Version   :  na                                                              *
#* Date      :  11 Feb 2022                                                     *
#* Website   :  na                                                              *
#* Copyright :  Damir Bakiev 2022-2022                                          *
#* License:                                                                     *
#* Use, modification & distribution is subject to Boost Software License Ver 1. *
#* http://www.boost.org/LICENSE_1_0.txt                                         *
#*******************************************************************************/
cmake_minimum_required(VERSION 3.16)

#set(PROJECT_NAME "GGEasy")

#project(${PROJECT_NAME} LANGUAGES CXX)
project(ThermCtrlSubzero_IRT5502)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin2)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

string(TIMESTAMP DT "%d.%m.%Y - %H:%M:%S")# [<format string>] [UTC])
add_definitions(
    -DBUILD_DATE="${DT}"
#    -DEL_EMU=1
    )

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
    #    Concurrent
    Core
    Gui
    LinguistTools
    OpenGL
    PrintSupport
    Widgets
    SerialPort
    REQUIRED
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS OpenGLWidgets REQUIRED)
endif()

include_directories (
    ${INCLUDE_DIRECTORIES}
    CommonInterfaces
    ElemerDevice
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations ${TS_FILES})
    qt6_add_resources(RESOURCES rc.qrc res/resources.qrc)
else()
    qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations ${TS_FILES})
    qt5_add_resources(RESOURCES rc.qrc res/resources.qrc)
endif()

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/myapp.rc")

set(SOURCE
    ${SOURCE}
    automatic.cpp
    axistag.cpp
    customplot.cpp
    irt5502.cpp
    main.cpp
    mainwindow.cpp
    pointmodel.cpp
    qcustomplot.cpp
    thermctrl.cpp
    ElemerDevice/ed_device.cpp
    ElemerDevice/ed_port.cpp
    )

set(HEADERS
    ${HEADERS}
    automatic.h
    axistag.h
    customplot.h
    doublespinbox.h
    irt5502.h
    mainwindow.h
    pointmodel.h
    qcustomplot.h
    thermctrl.h
    version.h
    CommonInterfaces/commoninterfaces.h
    ElemerDevice/ed_common_types.h
    ElemerDevice/ed_device.h
    ElemerDevice/ed_port.h
    ElemerDevice/ed_utils.h
    )

set(UI
    ${UI}
    mainwindow.ui
    thermctrl.ui
    )

add_executable(${PROJECT_NAME}
    ${TS_FILES}
    ${SOURCE}
    ${HEADERS}
    ${UI}
    ${RESOURCES}
    ${APP_ICON_RESOURCE_WINDOWS}
    ${MAGIC_GET}
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
    #    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Widgets
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${OUTPUT_DIRECTORY})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE setupapi)
    # windeployqt
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    get_target_property(_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_QT_BIN_DIR "${_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_QT_BIN_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_QT_BIN_DIR}" "${WINDEPLOYQT_EXECUTABLE}"
        --verbose 0
        --no-compiler-runtime
        \"$<TARGET_FILE:${PROJECT_NAME}>\"
        COMMENT "Deploying Qt..."
        )
    set(GUI_TYPE WIN32)

endif()


set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}${SUFFIX}")


if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
        LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
        )
endif()


set(TARGETS
    ${PROJECT_NAME}
    )

foreach(TNAME ${TARGETS})
    target_compile_features(${TNAME} PUBLIC  cxx_std_20)
    #  target_include_directories(${TNAME} PUBLIC  ${CMAKE_CURRENT_BINARY_DIR})
    #  target_include_directories(${TNAME} PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()
