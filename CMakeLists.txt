cmake_minimum_required(VERSION 3.21)

project(SpeedCurveFittingTool VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(speed_curve_fitting_tool
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/curvecanvas.cpp
    src/curvecanvas.h
    src/sliderpanel.cpp
    src/sliderpanel.h
    src/motionslider.cpp
    src/motionslider.h
    src/resultpanel.cpp
    src/resultpanel.h
    src/skinprovider.cpp
    src/skinprovider.h
    src/restorablesplitter.cpp
    src/restorablesplitter.h
)

target_link_libraries(speed_curve_fitting_tool PRIVATE Qt6::Widgets)

if(WIN32)
    set_target_properties(speed_curve_fitting_tool PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
