cmake_minimum_required(VERSION 3.21)

project(TaxiGraph VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)
qt_standard_project_setup()

# Las clases se declaran en .h y se implementan en .cpp. Qt genera el codigo de los formularios .ui.
qt_add_executable(TaxiGraph
    main.cpp
    Nodo.h
    Nodo.cpp
    Arista.h
    Arista.cpp
    ElementoNodo.h
    ElementoNodo.cpp
    ListaNodos.h
    ListaNodos.cpp
    ElementoArista.h
    ElementoArista.cpp
    ListaAristas.h
    ListaAristas.cpp
    NodoCola.h
    NodoCola.cpp
    Cola.h
    Cola.cpp
    NodoPila.h
    NodoPila.cpp
    Pila.h
    Pila.cpp
    ResultadoRuta.h
    ResultadoRuta.cpp
    Pasajero.h
    Pasajero.cpp
    ManzanaCiudad.h
    ManzanaCiudad.cpp
    TextoFlotante.h
    TextoFlotante.cpp
    Grafo.h
    Grafo.cpp
    RelojSimulacion.h
    RelojSimulacion.cpp
    Taxi.h
    Taxi.cpp
    TaxiBFS.h
    TaxiBFS.cpp
    TaxiDFS.h
    TaxiDFS.cpp
    TaxiDijkstra.h
    TaxiDijkstra.cpp
    TaxiManual.h
    TaxiManual.cpp
    Simulacion.h
    Simulacion.cpp
    MapaWidget.h
    MapaWidget.cpp
    BarrasGananciasWidget.h
    BarrasGananciasWidget.cpp
    GraficoGananciasWidget.h
    GraficoGananciasWidget.cpp
    DialogoNodo.h
    DialogoNodo.cpp
    MainWindow.h
    MainWindow.cpp
    TransicionesPanel.h
    mainwindow.ui
    nodedialog.ui
)

if(MSVC)
    target_compile_options(TaxiGraph PRIVATE /utf-8)
endif()

qt_add_resources(TaxiGraph "interfaz"
    PREFIX "/"
    FILES
        interfaz.qss
        flecha-selector.png
)

target_link_libraries(TaxiGraph PRIVATE Qt6::Widgets Qt6::Charts)

set_target_properties(TaxiGraph PROPERTIES
    WIN32_EXECUTABLE TRUE
)
