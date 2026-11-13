cmake_minimum_required(VERSION 3.21)

project(TaxiGraph VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)
qt_standard_project_setup()

# Todo el codigo C++ vive en main.cpp; los formularios estan en los archivos .ui
qt_add_executable(TaxiGraph
    main.cpp
    mainwindow.ui
    nodedialog.ui
)

if(MSVC)
    target_compile_options(TaxiGraph PRIVATE /utf-8)
endif()

target_link_libraries(TaxiGraph PRIVATE Qt6::Widgets Qt6::Charts)

set_target_properties(TaxiGraph PROPERTIES
    WIN32_EXECUTABLE TRUE
)
