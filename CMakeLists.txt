cmake_minimum_required(VERSION 3.21)

project(OnDrive VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)
qt_standard_project_setup()

qt_add_executable(OnDrive
    src/estructuras/Cola.cpp
    src/estructuras/Cola.h
    src/estructuras/ElementoArista.cpp
    src/estructuras/ElementoArista.h
    src/estructuras/ElementoNodo.cpp
    src/estructuras/ElementoNodo.h
    src/estructuras/ListaAristas.cpp
    src/estructuras/ListaAristas.h
    src/estructuras/ListaNodos.cpp
    src/estructuras/ListaNodos.h
    src/estructuras/NodoCola.cpp
    src/estructuras/NodoCola.h
    src/estructuras/NodoPila.cpp
    src/estructuras/NodoPila.h
    src/estructuras/Pila.cpp
    src/estructuras/Pila.h
    src/interfaz/BarrasGananciasWidget.cpp
    src/interfaz/BarrasGananciasWidget.h
    src/interfaz/DialogoNodo.cpp
    src/interfaz/DialogoNodo.h
    src/interfaz/GraficoGananciasWidget.cpp
    src/interfaz/GraficoGananciasWidget.h
    src/interfaz/MainWindow.cpp
    src/interfaz/MainWindow.h
    src/interfaz/MapaWidget.cpp
    src/interfaz/MapaWidget.h
    src/interfaz/TextoFlotante.cpp
    src/interfaz/TextoFlotante.h
    src/main.cpp
    src/modelo/Arista.cpp
    src/modelo/Arista.h
    src/modelo/ManzanaCiudad.cpp
    src/modelo/ManzanaCiudad.h
    src/modelo/Nodo.cpp
    src/modelo/Nodo.h
    src/modelo/Pasajero.cpp
    src/modelo/Pasajero.h
    src/modelo/ResultadoRuta.cpp
    src/modelo/ResultadoRuta.h
    src/simulacion/Grafo.cpp
    src/simulacion/Grafo.h
    src/simulacion/RelojSimulacion.cpp
    src/simulacion/RelojSimulacion.h
    src/simulacion/Simulacion.cpp
    src/simulacion/Simulacion.h
    src/taxis/Taxi.cpp
    src/taxis/Taxi.h
    src/taxis/TaxiBFS.cpp
    src/taxis/TaxiBFS.h
    src/taxis/TaxiDFS.cpp
    src/taxis/TaxiDFS.h
    src/taxis/TaxiDijkstra.cpp
    src/taxis/TaxiDijkstra.h
    src/taxis/TaxiManual.cpp
    src/taxis/TaxiManual.h
    ui/mainwindow.ui
    ui/nodedialog.ui
)

target_include_directories(OnDrive PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

if(MSVC)
    target_compile_options(OnDrive PRIVATE /utf-8)
endif()

qt_add_resources(OnDrive "interfaz"
    PREFIX "/"
    BASE "recursos"
    FILES
        recursos/interfaz.qss
        recursos/flecha-selector.png
)

target_link_libraries(OnDrive PRIVATE Qt6::Widgets Qt6::Charts)

set_target_properties(OnDrive PROPERTIES
    WIN32_EXECUTABLE TRUE
)
