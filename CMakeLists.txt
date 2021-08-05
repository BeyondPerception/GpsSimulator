cmake_minimum_required(VERSION 3.19)
project(gps_sim_gui)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOUIC_SEARCH_PATHS form/)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets pthread loguru dl gps)

link_directories(lib/loguru/bin)
include_directories(lib/loguru/include/)

add_executable(${PROJECT_NAME} src/main.cpp src/HackRfController.cpp src/GpsSdrSim.cpp src/gui/MainWindow.cpp
        include/gui/MainWindow.hpp src/gui/QStatusButton.cpp include/gui/QStatusButton.hpp include/gui/QLogButton.hpp
        src/gui/QLogButton.cpp include/gui/QDebugStream.hpp include/gui/QHackRfButton.hpp src/gui/QHackRfButton.cpp
        include/gui/QGpsSimButton.hpp src/gui/QGpsSimButton.cpp include/gui/QGpsReceiver.hpp src/gui/QGpsReceiver.cpp)

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
target_include_directories(${PROJECT_NAME} PRIVATE include/)
target_include_directories(${PROJECT_NAME} PRIVATE include/gui)
