cmake_minimum_required(VERSION 3.16)

project(capturepin VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# set(CMAKE_BUILD_RPATH "/home/lhz/Qt/6.8.0/gcc_64/lib")
# set(CMAKE_INSTALL_RPATH "/opt/capturepin/lib")
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS DBus Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(capturepin
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        request.h request.cpp
        screenshot.h screenshot.cpp
        start_shot.h start_shot.cpp start_shot.ui
        resource.qrc
        pin.h pin.cpp pin.ui
        keyhandler.h keyhandler.cpp
        reactcmd.h reactcmd.cpp
        textbox.h textbox.cpp
        toolbar.h toolbar.cpp toolbar.ui
        hyprsocket.h hyprsocket.cpp
        sharevar.h sharevar.cpp
        graphview.h graphview.cpp
        colorgridwidget.h colorgridwidget.cpp






    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET capturepin APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(capturepin SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(capturepin
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(capturepin PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::DBus Qt6::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.capturepin)
endif()
set_target_properties(capturepin PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS capturepin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(capturepin)
endif()
