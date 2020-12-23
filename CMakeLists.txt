cmake_minimum_required(VERSION 3.18.2)
project(om-camera VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt5 5.15.0 CONFIG REQUIRED COMPONENTS Core Gui Widgets DBus)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)

# FIXME why doesn't anything set QT_INCLUDES?
include_directories(/usr/include/qt5 /usr/include/qt5/QtCore /usr/include/qt5/QtDBus /usr/include/qt5/QtGui /usr/include/qt5/QtWidgets)

add_subdirectory(src)
