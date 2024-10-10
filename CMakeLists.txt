
cmake_minimum_required(VERSION 3.27)
project(DIAN-Capture)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/Qt/5.15.2/mingw81_64")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

include_directories(.)
include_directories("third party")
include_directories("third party/glog")
include_directories("third party/glog/include")
include_directories("third party/glog/include/glog")

add_executable(DIAN-Capture
        "third party/glog/include/glog/log_severity.h"
        "third party/glog/include/glog/vlog_is_on.h"
        "third party/glog/include/logging.h"
        "third party/glog/include/raw_logging.h"
        "third party/glog/include/stl_logging.h"
        ColorItem.cpp
        ColorItem.h
        imageview.cpp
        imageview.h
        LinePaint.cpp
        LinePaint.h
        main.cpp
        PointSizeWidget.cpp
        PointSizeWidget.h
        qxtglobal.h
        qxtglobalshortcut.cpp
        qxtglobalshortcut.h
        qxtglobalshortcut_p.h
        qxtglobalshortcut_win.cpp
        RectPaint.cpp
        RectPaint.h
        screenview.cpp
        screenview.h
        CaptureApplication.cpp
        CaptureApplication.h
        struct.h
        TextPaint.cpp
        TextPaint.h)

target_link_libraries(DIAN-Capture
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()