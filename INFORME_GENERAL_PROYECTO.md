# Informe General de Arquitectura, Estado Actual y Código Fuente
**Proyecto:** Simulador de Taxis en Grafos (*TaxiGraph*)  
**Rama Git Activa:** rontend/taxigraph-ui  
**Tecnologías:** C++17/20, Qt 6 (Widgets, Charts), CMake, MinGW 64-bit  
**Fecha:** Noviembre 2026  

---

## 1. Resumen Ejecutivo y Propósito de este Informe

Este documento fue generado para proporcionar un **volcado completo, estructurado y detallado** de todo el código fuente del proyecto tras dos bloques críticos de modificaciones:
1. **Rediseño y modernización de la interfaz de usuario (Frontend/UI):**
   - Separación de la hoja de estilos en un archivo de recursos Qt dedicado (interfaz.qss).
   - Configuración de paleta visual oscura y tipografía Segoe UI en main.cpp.
   - Ajustes en el layout y controles en mainwindow.ui y 
odedialog.ui.
   - Mejoras de renderizado y visualización en MapaWidget, MainWindow, GraficoGananciasWidget y BarrasGananciasWidget.
2. **Migración de #pragma once a guardas de inclusión estándar ISO C++ (#ifndef / #define / #endif):**
   - Reemplazo en los **27 archivos de cabecera (.h)**.
   - Garantiza 100% de compatibilidad con estándares académicos universitarios sin alterar la lógica de compilación.

El proyecto **compila al 100% sin errores ni advertencias** con Qt 6 y MinGW.

---

## 2. Estado de Git y Cambios Pendientes de Commit

El repositorio se encuentra en la rama rontend/taxigraph-ui con cambios de diseño y las guardas de cabecera listos para revisión:

`	ext
Changes not staged for commit:
  Arista.h, BarrasGananciasWidget.h/.cpp, Cola.h, DialogoNodo.h/.cpp,
  ElementoArista.h/.cpp, ElementoNodo.h/.cpp, GraficoGananciasWidget.h/.cpp,
  Grafo.h, ListaAristas.h, ListaNodos.h, MainWindow.h/.cpp, ManzanaCiudad.h,
  MapaWidget.h/.cpp, Nodo.h, NodoCola.h, NodoPila.h, Pasajero.h, Pila.h,
  RelojSimulacion.h, ResultadoRuta.h, Simulacion.h, Taxi.h, TaxiBFS.h,
  TaxiDFS.h, TaxiDijkstra.h, TaxiManual.h, TextoFlotante.h,
  main.cpp, mainwindow.ui, nodedialog.ui, CMakeLists.txt

Untracked files:
  interfaz.qss
`

---

## 3. Arquitectura del Sistema

El simulador implementa un modelo de ciudad basado en teoría de grafos:
- **Estructuras de datos dinámicas manuales:** En lugar de usar únicamente contenedores STL, se implementaron listas enlazadas simples (ListaNodos, ListaAristas), una Pila (Pila, NodoPila) y una Cola (Cola, NodoCola) para propósitos de modelado y evaluación académica.
- **Grafo:** Nodos (intersecciones) y Aristas ponderadas (calles/avenidas con tiempo, distancia y sentidos).
- **Algoritmos de búsqueda de rutas:**
  - **BFS (Breadth-First Search):** Encuentra la ruta con menor número de saltos/aristas.
  - **DFS (Depth-First Search):** Búsqueda en profundidad (estrategia alternativa o de exploración profunda).
  - **Dijkstra:** Encuentra la ruta óptima de menor costo ponderado (tiempo/distancia).
  - **Manual:** Permite al usuario guiar el taxi paso a paso.
- **Simulación y Taxímetro:** Reloj día/noche con factores de tráfico, cálculo de tarifas dinámicas (banderazo base, saltos, tiempo, recargos nocturnos, de aeropuerto, terminales, etc.).
- **Vistas y Monitores:** MapaWidget para renderizado en 2D de la ciudad y animación de vehículos; gráficos de ganancias por taxi (BarrasGananciasWidget y GraficoGananciasWidget).

---

## 4. Código Fuente Completo del Proyecto

A continuación se presenta el código íntegro de cada uno de los archivos del sistema, clasificados por módulos funcionales:


---

## 1. Configuración, Construcción y Estilos del Sistema

### Archivo: CMakeLists.txt

`cmake
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
    mainwindow.ui
    nodedialog.ui
)

if(MSVC)
    target_compile_options(TaxiGraph PRIVATE /utf-8)
endif()

qt_add_resources(TaxiGraph "interfaz"
    PREFIX "/"
    FILES interfaz.qss
)

target_link_libraries(TaxiGraph PRIVATE Qt6::Widgets Qt6::Charts)

set_target_properties(TaxiGraph PROPERTIES
    WIN32_EXECUTABLE TRUE
)

`

### Archivo: main.cpp

`cpp
#include "MainWindow.h"

#include <QApplication>
#include <QColor>
#include <QFile>
#include <QFont>
#include <QPalette>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("TaxiGraph"));
    app.setStyle(QStringLiteral("Fusion"));
    app.setFont(QFont(QStringLiteral("Segoe UI"), 10));

    QPalette paleta;
    paleta.setColor(QPalette::Window, QColor("#101821"));
    paleta.setColor(QPalette::WindowText, QColor("#e8eef4"));
    paleta.setColor(QPalette::Base, QColor("#131d28"));
    paleta.setColor(QPalette::AlternateBase, QColor("#1b2835"));
    paleta.setColor(QPalette::Text, QColor("#e8eef4"));
    paleta.setColor(QPalette::Button, QColor("#202e3d"));
    paleta.setColor(QPalette::ButtonText, QColor("#e8eef4"));
    paleta.setColor(QPalette::Highlight, QColor("#38536b"));
    paleta.setColor(QPalette::HighlightedText, Qt::white);
    paleta.setColor(QPalette::Disabled, QPalette::Text, QColor("#81909f"));
    paleta.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#81909f"));
    paleta.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#81909f"));
    app.setPalette(paleta);

    QFile tema(QStringLiteral(":/interfaz.qss"));
    if (tema.open(QIODevice::ReadOnly))
    {
        app.setStyleSheet(QString::fromUtf8(tema.readAll()));
    }

    MainWindow ventana;
    ventana.show();
    return app.exec();
}

`

### Archivo: interfaz.qss

`css
/* Surfaces and controls shared by the simulator and its dialogs. */
QMainWindow, QDialog {
    background: #101821;
}
QWidget {
    color: #e8eef4;
}
QWidget#appHeader {
    background: #151f29;
    border-bottom: 1px solid #415264;
}
QWidget#graphSummary, QWidget#graphLegend, QGroupBox#groupEdit {
    background: #17222e;
    border: 1px solid #2b3a49;
    border-radius: 3px;
}
QWidget#graphSummary {
    border-left: 3px solid #ffd166;
}
QWidget#manualSummary {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #212d37, stop:1 #19242f);
    border-left: 2px solid #ffd166;
    border-bottom: 1px solid #354452;
}
QGroupBox#groupSim {
    border: 0;
}
QWidget#sidePanel {
    background: #17222e;
    border: 1px solid #2b3a49;
    border-radius: 3px;
}
QLabel#lblProduct, QLabel#lblGraphTitle, QLabel#lblTelemetry, QLabel#lblDialogTitle {
    color: #f4f7fa;
    font-family: "Bahnschrift", "Segoe UI";
    font-weight: 600;
}
QLabel#lblProduct {
    font-size: 23px;
}
QLabel#lblGraphTitle, QLabel#lblTelemetry, QLabel#lblDialogTitle {
    font-size: 18px;
}
QLabel#lblGraphCounts, QLabel#lblGraphClock, QLabel#lblManualIncome {
    font-family: "Consolas", monospace;
}
QLabel#lblGraphClock {
    font-size: 19px;
    font-weight: 700;
}
QLabel#lblManualIncome {
    font-size: 17px;
    font-weight: 700;
    color: #ffd166;
}
QLabel#lblManualTitle, QLabel#lblNodeFocus {
    color: #f0d6a1;
    font-weight: 600;
}
QLabel#lblGraphCounts, QLabel#lblGraphPeriod, QLabel#lblManualTrips, QLabel#lblManualState,
QLabel#lblNodeKey, QLabel#lblOneWayKey, QLabel#lblTwoWayKey {
    color: #a5b4c3;
    font-size: 12px;
}
QLabel#lblBfs, QLabel#lblDfs, QLabel#lblDijkstra {
    background: #1e2b36;
    border-left: 2px solid #8cd7ee;
    padding: 5px 8px;
    font-size: 12px;
}
QLabel#lblDfs { border-left-color: #ff79a6; }
QLabel#lblDijkstra { border-left-color: #81e1a1; }
QLabel#lblEditHint, QLabel#lblMapMode, QLabel#lblSpeed, QLabel#lblTimeMode {
    color: #a5b4c3;
}
QLabel#lblEditHint {
    font-size: 12px;
}
QPushButton, QComboBox, QLineEdit {
    background: #202e3d;
    border: 1px solid #35485b;
    border-radius: 4px;
    min-height: 19px;
    padding: 6px 10px;
}
QPushButton {
    font-weight: 600;
}
QPushButton:hover, QComboBox:hover, QLineEdit:hover {
    background: #28394b;
    border-color: #5a748c;
}
QPushButton:focus, QComboBox:focus, QLineEdit:focus {
    border-color: #a9c7e2;
}
QPushButton:pressed {
    background: #17222e;
}
QPushButton#btnPause {
    min-width: 82px;
}
QPushButton#btnPause, QDialogButtonBox QPushButton:default {
    background: #ffd166;
    border-color: #ffd166;
    color: #222016;
}
QPushButton#btnPause:hover, QDialogButtonBox QPushButton:default:hover {
    background: #ffdf93;
}
QPushButton#btnPause:focus, QDialogButtonBox QPushButton:default:focus {
    border-color: #ffffff;
}
QPushButton#btnPause:checked {
    background: #2c3b47;
    border-color: #ffd166;
    color: #ffe6a6;
}
QPushButton:disabled {
    background: #192530;
    border-color: #2b3a49;
    color: #81909f;
}
QCheckBox {
    spacing: 7px;
    color: #c5d2de;
}
QComboBox {
    padding-right: 24px;
}
QComboBox QAbstractItemView {
    background: #202e3d;
    border: 1px solid #35485b;
    selection-background-color: #38536b;
    selection-color: #ffffff;
    outline: none;
}
QTabWidget::pane {
    background: #17222e;
    border: 0;
    border-top: 1px solid #35485b;
}
QTabBar::tab {
    background: #17222e;
    color: #aebdcb;
    border: 0;
    border-bottom: 2px solid transparent;
    padding: 8px 7px 7px;
}
QTabBar::tab:hover {
    color: #f4f7fa;
    background: #243242;
}
QTabBar::tab:selected {
    border-bottom-color: #ffd166;
    color: #f8e4af;
    font-weight: 600;
}
QTableWidget {
    background: #131d28;
    alternate-background-color: #1b2835;
    border: 1px solid #2b3a49;
    border-radius: 3px;
    selection-background-color: #38536b;
}
QTableWidget::item {
    padding-left: 5px;
    padding-right: 5px;
}
QHeaderView::section {
    background: #223242;
    color: #d7e2ec;
    border: 0;
    border-bottom: 1px solid #35485b;
    padding: 6px 4px;
    font-size: 12px;
    font-weight: 600;
}
QTextEdit, QTextBrowser {
    background: #131d28;
    border: 1px solid #2b3a49;
    border-radius: 4px;
    color: #d9e4ee;
    padding: 8px;
}
QSplitter::handle {
    background: #263645;
    width: 6px;
}
QSplitter::handle:hover {
    background: #57718a;
}
QStatusBar {
    background: #101821;
    color: #a5b4c3;
}
QStatusBar::item {
    border: 0;
}
QToolTip {
    background: #263645;
    color: #f4f7fa;
    border: 1px solid #5a748c;
    padding: 4px;
}

`

### Archivo: Main.qml

`qml
import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
}

`


---

## 2. Estructuras de Datos Propias (Listas, Pila, Cola, Nodos)

### Archivo: Nodo.h

`cpp
#ifndef NODO_H
#define NODO_H

#include <QPointF>
#include <QString>

class Nodo
{
private:
    int id;
    QString nombre;
    int x;
    int y;
    QString tipo; // "Normal", "Aeropuerto", "Terminal", "Turistico"

public:
    Nodo(int id, QString nombre, int x, int y, QString tipo);

    int getId() const;
    QString getNombre() const;
    int getX() const;
    int getY() const;
    QPointF getPosicion() const;
    QString getTipo() const;

    void setId(int id);
    void setNombre(QString nombre);
    void setX(int x);
    void setY(int y);
    void setTipo(QString tipo);
};

#endif // NODO_H

`

### Archivo: Nodo.cpp

`cpp
#include "Nodo.h"

Nodo::Nodo(int id, QString nombre, int x, int y, QString tipo)
{
    this->id = id;
    this->nombre = nombre;
    this->x = x;
    this->y = y;
    this->tipo = tipo;
}

int Nodo::getId() const
{
    return id;
}

QString Nodo::getNombre() const
{
    return nombre;
}

int Nodo::getX() const
{
    return x;
}

int Nodo::getY() const
{
    return y;
}

QPointF Nodo::getPosicion() const
{
    return QPointF(x, y);
}

QString Nodo::getTipo() const
{
    return tipo;
}

void Nodo::setId(int id)
{
    this->id = id;
}

void Nodo::setNombre(QString nombre)
{
    this->nombre = nombre;
}

void Nodo::setX(int x)
{
    this->x = x;
}

void Nodo::setY(int y)
{
    this->y = y;
}

void Nodo::setTipo(QString tipo)
{
    this->tipo = tipo;
}

`

### Archivo: ElementoNodo.h

`cpp
#ifndef ELEMENTO_NODO_H
#define ELEMENTO_NODO_H

class Nodo;

class ElementoNodo
{
private:
    Nodo *nodo;
    ElementoNodo *siguiente;

public:
    ElementoNodo(Nodo *nodo);

    Nodo *getNodo() const;
    ElementoNodo *getSiguiente() const;
    void setSiguiente(ElementoNodo *siguiente);
};

#endif // ELEMENTO_NODO_H

`

### Archivo: ElementoNodo.cpp

`cpp
#include "ElementoNodo.h"

ElementoNodo::ElementoNodo(Nodo *nodo)
{
    this->nodo = nodo;
    this->siguiente = nullptr;
}

Nodo *ElementoNodo::getNodo() const
{
    return nodo;
}

ElementoNodo *ElementoNodo::getSiguiente() const
{
    return siguiente;
}

void ElementoNodo::setSiguiente(ElementoNodo *siguiente)
{
    this->siguiente = siguiente;
}

`

### Archivo: ListaNodos.h

`cpp
#ifndef LISTA_NODOS_H
#define LISTA_NODOS_H

class ElementoNodo;
class Nodo;

class ListaNodos
{
private:
    ElementoNodo *primero;
    int cantidad;

public:
    ListaNodos();

    ~ListaNodos();

    void agregar(Nodo *nodo);

    Nodo *buscarPorId(int id) const;

    bool eliminarPorId(int id);

    ElementoNodo *getPrimero() const;
    int getCantidad() const;

    void limpiar();
};

#endif // LISTA_NODOS_H

`

### Archivo: ListaNodos.cpp

`cpp
#include "ListaNodos.h"

#include "ElementoNodo.h"
#include "Nodo.h"

ListaNodos::ListaNodos()
{
    primero = nullptr;
    cantidad = 0;
}

ListaNodos::~ListaNodos()
{
    limpiar();
}

void ListaNodos::agregar(Nodo *nodo)
{
    if (nodo == nullptr)
    {
        return;
    }

    ElementoNodo *nuevo = new ElementoNodo(nodo);

    if (primero == nullptr)
    {
        primero = nuevo;
    }
    else
    {
        ElementoNodo *actual = primero;
        while (actual->getSiguiente() != nullptr)
        {
            actual = actual->getSiguiente();
        }
        actual->setSiguiente(nuevo);
    }

    cantidad++;
}

Nodo *ListaNodos::buscarPorId(int id) const
{
    ElementoNodo *actual = primero;

    while (actual != nullptr)
    {
        Nodo *nodo = actual->getNodo();
        if (nodo != nullptr && nodo->getId() == id)
        {
            return nodo;
        }
        actual = actual->getSiguiente();
    }

    return nullptr;
}

bool ListaNodos::eliminarPorId(int id)
{
    ElementoNodo *actual = primero;
    ElementoNodo *anterior = nullptr;

    while (actual != nullptr)
    {
        Nodo *nodo = actual->getNodo();

        if (nodo != nullptr && nodo->getId() == id)
        {
            if (anterior == nullptr)
            {
                primero = actual->getSiguiente();
            }
            else
            {
                anterior->setSiguiente(actual->getSiguiente());
            }

            delete actual->getNodo();
            delete actual;
            cantidad--;
            return true;
        }

        anterior = actual;
        actual = actual->getSiguiente();
    }

    return false;
}

ElementoNodo *ListaNodos::getPrimero() const
{
    return primero;
}

int ListaNodos::getCantidad() const
{
    return cantidad;
}

void ListaNodos::limpiar()
{
    ElementoNodo *actual = primero;

    while (actual != nullptr)
    {
        ElementoNodo *siguiente = actual->getSiguiente();
        delete actual->getNodo();
        delete actual;
        actual = siguiente;
    }

    primero = nullptr;
    cantidad = 0;
}

`

### Archivo: Arista.h

`cpp
#ifndef ARISTA_H
#define ARISTA_H

class Arista
{
private:
    int origen;
    int destino;
    double peso;
    double pesoInicial;
    bool esAvenida;

public:
    Arista(int origen, int destino, double peso, bool esAvenida);

    int getOrigen() const;
    int getDestino() const;
    double getPeso() const;
    double getPesoInicial() const;
    bool getEsAvenida() const;

    void setOrigen(int origen);
    void setDestino(int destino);
    void setPeso(double peso);
    void setPesoInicial(double pesoInicial);
    void setEsAvenida(bool esAvenida);
};

#endif // ARISTA_H

`

### Archivo: Arista.cpp

`cpp
#include "Arista.h"

Arista::Arista(int origen, int destino, double peso, bool esAvenida)
{
    this->origen = origen;
    this->destino = destino;
    this->peso = peso;
    this->pesoInicial = peso;
    this->esAvenida = esAvenida;
}

int Arista::getOrigen() const
{
    return origen;
}

int Arista::getDestino() const
{
    return destino;
}

double Arista::getPeso() const
{
    return peso;
}

double Arista::getPesoInicial() const
{
    return pesoInicial;
}

bool Arista::getEsAvenida() const
{
    return esAvenida;
}

void Arista::setOrigen(int origen)
{
    this->origen = origen;
}

void Arista::setDestino(int destino)
{
    this->destino = destino;
}

void Arista::setPeso(double peso)
{
    this->peso = peso;
}

void Arista::setPesoInicial(double pesoInicial)
{
    this->pesoInicial = pesoInicial;
}

void Arista::setEsAvenida(bool esAvenida)
{
    this->esAvenida = esAvenida;
}

`

### Archivo: ElementoArista.h

`cpp
#ifndef ELEMENTO_ARISTA_H
#define ELEMENTO_ARISTA_H

class Arista;

class ElementoArista
{
private:
    Arista *arista;
    ElementoArista *siguiente;

public:
    ElementoArista(Arista *arista);

    Arista *getArista() const;
    ElementoArista *getSiguiente() const;
    void setSiguiente(ElementoArista *siguiente);
};

#endif // ELEMENTO_ARISTA_H

`

### Archivo: ElementoArista.cpp

`cpp
#include "ElementoArista.h"

ElementoArista::ElementoArista(Arista *arista)
{
    this->arista = arista;
    this->siguiente = nullptr;
}

Arista *ElementoArista::getArista() const
{
    return arista;
}

ElementoArista *ElementoArista::getSiguiente() const
{
    return siguiente;
}

void ElementoArista::setSiguiente(ElementoArista *siguiente)
{
    this->siguiente = siguiente;
}

`

### Archivo: ListaAristas.h

`cpp
#ifndef LISTA_ARISTAS_H
#define LISTA_ARISTAS_H

class Arista;
class ElementoArista;

class ListaAristas
{
private:
    ElementoArista *primero;
    int cantidad;

public:
    ListaAristas();

    ~ListaAristas();

    void agregar(Arista *arista);

    Arista *obtenerArista(int origen, int destino) const;

    bool existeArista(int origen, int destino) const;

    bool eliminarArista(int origen, int destino);

    void eliminarAristasDeNodo(int nodoId);

    ElementoArista *getPrimero() const;
    int getCantidad() const;

    void limpiar();
};

#endif // LISTA_ARISTAS_H

`

### Archivo: ListaAristas.cpp

`cpp
#include "ListaAristas.h"

#include "Arista.h"
#include "ElementoArista.h"

ListaAristas::ListaAristas()
{
    primero = nullptr;
    cantidad = 0;
}

ListaAristas::~ListaAristas()
{
    limpiar();
}

void ListaAristas::agregar(Arista *arista)
{
    if (arista == nullptr)
    {
        return;
    }

    ElementoArista *nuevo = new ElementoArista(arista);

    if (primero == nullptr)
    {
        primero = nuevo;
    }
    else
    {
        ElementoArista *actual = primero;
        while (actual->getSiguiente() != nullptr)
        {
            actual = actual->getSiguiente();
        }
        actual->setSiguiente(nuevo);
    }

    cantidad++;
}

Arista *ListaAristas::obtenerArista(int origen, int destino) const
{
    ElementoArista *actual = primero;

    while (actual != nullptr)
    {
        Arista *arista = actual->getArista();
        if (arista != nullptr && arista->getOrigen() == origen && arista->getDestino() == destino)
        {
            return arista;
        }
        actual = actual->getSiguiente();
    }

    return nullptr;
}

bool ListaAristas::existeArista(int origen, int destino) const
{
    return obtenerArista(origen, destino) != nullptr;
}

bool ListaAristas::eliminarArista(int origen, int destino)
{
    ElementoArista *actual = primero;
    ElementoArista *anterior = nullptr;

    while (actual != nullptr)
    {
        Arista *arista = actual->getArista();

        if (arista != nullptr && arista->getOrigen() == origen && arista->getDestino() == destino)
        {
            if (anterior == nullptr)
            {
                primero = actual->getSiguiente();
            }
            else
            {
                anterior->setSiguiente(actual->getSiguiente());
            }

            delete actual->getArista();
            delete actual;
            cantidad--;
            return true;
        }

        anterior = actual;
        actual = actual->getSiguiente();
    }

    return false;
}

void ListaAristas::eliminarAristasDeNodo(int nodoId)
{
    ElementoArista *actual = primero;
    ElementoArista *anterior = nullptr;

    while (actual != nullptr)
    {
        ElementoArista *siguiente = actual->getSiguiente();
        Arista *arista = actual->getArista();
        bool eliminar = false;

        if (arista != nullptr)
        {
            if (arista->getOrigen() == nodoId || arista->getDestino() == nodoId)
            {
                eliminar = true;
            }
        }

        if (eliminar)
        {
            if (anterior == nullptr)
            {
                primero = siguiente;
            }
            else
            {
                anterior->setSiguiente(siguiente);
            }

            delete actual->getArista();
            delete actual;
            cantidad--;
        }
        else
        {
            anterior = actual;
        }

        actual = siguiente;
    }
}

ElementoArista *ListaAristas::getPrimero() const
{
    return primero;
}

int ListaAristas::getCantidad() const
{
    return cantidad;
}

void ListaAristas::limpiar()
{
    ElementoArista *actual = primero;

    while (actual != nullptr)
    {
        ElementoArista *siguiente = actual->getSiguiente();
        delete actual->getArista();
        delete actual;
        actual = siguiente;
    }

    primero = nullptr;
    cantidad = 0;
}

`

### Archivo: NodoCola.h

`cpp
#ifndef NODO_COLA_H
#define NODO_COLA_H

class NodoCola
{
private:
    int dato;
    NodoCola *siguiente;

public:
    NodoCola(int dato);

    int getDato() const;
    NodoCola *getSiguiente() const;
    void setSiguiente(NodoCola *siguiente);
};

#endif // NODO_COLA_H

`

### Archivo: NodoCola.cpp

`cpp
#include "NodoCola.h"

NodoCola::NodoCola(int dato)
{
    this->dato = dato;
    this->siguiente = nullptr;
}

int NodoCola::getDato() const
{
    return dato;
}

NodoCola *NodoCola::getSiguiente() const
{
    return siguiente;
}

void NodoCola::setSiguiente(NodoCola *siguiente)
{
    this->siguiente = siguiente;
}

`

### Archivo: Cola.h

`cpp
#ifndef COLA_H
#define COLA_H

class NodoCola;

class Cola
{
private:
    NodoCola *frente;
    NodoCola *finalCola;

public:
    Cola();

    ~Cola();

    void encolar(int dato);

    int desencolar();

    bool estaVacia() const;

    void limpiar();
};

#endif // COLA_H

`

### Archivo: Cola.cpp

`cpp
#include "Cola.h"

#include "NodoCola.h"

Cola::Cola()
{
    frente = nullptr;
    finalCola = nullptr;
}

Cola::~Cola()
{
    limpiar();
}

void Cola::encolar(int dato)
{
    NodoCola *nuevo = new NodoCola(dato);

    if (estaVacia())
    {
        frente = nuevo;
        finalCola = nuevo;
    }
    else
    {
        finalCola->setSiguiente(nuevo);
        finalCola = nuevo;
    }
}

int Cola::desencolar()
{
    if (estaVacia())
    {
        return -1;
    }

    NodoCola *aux = frente;
    int dato = aux->getDato();

    frente = frente->getSiguiente();
    if (frente == nullptr)
    {
        finalCola = nullptr;
    }

    delete aux;
    return dato;
}

bool Cola::estaVacia() const
{
    return frente == nullptr;
}

void Cola::limpiar()
{
    while (!estaVacia())
    {
        desencolar();
    }
}

`

### Archivo: NodoPila.h

`cpp
#ifndef NODO_PILA_H
#define NODO_PILA_H

class NodoPila
{
private:
    int dato;
    NodoPila *siguiente;

public:
    NodoPila(int dato);

    int getDato() const;
    NodoPila *getSiguiente() const;
    void setSiguiente(NodoPila *siguiente);
};

#endif // NODO_PILA_H

`

### Archivo: NodoPila.cpp

`cpp
#include "NodoPila.h"

NodoPila::NodoPila(int dato)
{
    this->dato = dato;
    this->siguiente = nullptr;
}

int NodoPila::getDato() const
{
    return dato;
}

NodoPila *NodoPila::getSiguiente() const
{
    return siguiente;
}

void NodoPila::setSiguiente(NodoPila *siguiente)
{
    this->siguiente = siguiente;
}

`

### Archivo: Pila.h

`cpp
#ifndef PILA_H
#define PILA_H

class NodoPila;

class Pila
{
private:
    NodoPila *cima;

public:
    Pila();

    ~Pila();

    void apilar(int dato);

    int desapilar();

    bool estaVacia() const;

    void limpiar();
};

#endif // PILA_H

`

### Archivo: Pila.cpp

`cpp
#include "Pila.h"

#include "NodoPila.h"

Pila::Pila()
{
    cima = nullptr;
}

Pila::~Pila()
{
    limpiar();
}

void Pila::apilar(int dato)
{
    NodoPila *nuevo = new NodoPila(dato);
    nuevo->setSiguiente(cima);
    cima = nuevo;
}

int Pila::desapilar()
{
    if (estaVacia())
    {
        return -1;
    }

    NodoPila *aux = cima;
    int dato = aux->getDato();
    cima = cima->getSiguiente();
    delete aux;
    return dato;
}

bool Pila::estaVacia() const
{
    return cima == nullptr;
}

void Pila::limpiar()
{
    while (!estaVacia())
    {
        desapilar();
    }
}

`


---

## 3. Lógica del Grafo y Algoritmos de Búsqueda

### Archivo: Grafo.h

`cpp
#ifndef GRAFO_H
#define GRAFO_H

#include <QObject>
#include <QPointF>
#include <QString>

#include "ResultadoRuta.h"

class Arista;
class ListaAristas;
class ListaNodos;
class Nodo;

class Grafo : public QObject
{
    Q_OBJECT

private:
    ListaNodos *nodos;
    ListaAristas *aristas;
    double multiplicadorHoraPico;

    ResultadoRuta reconstruirRuta(int anterior[], int origen, int destino, int explorados) const;

public:
    explicit Grafo(QObject *parent = nullptr);

    ~Grafo() override;

    ListaNodos *getNodos() const;
    ListaAristas *getAristas() const;

    Nodo *buscarNodo(int id) const;

    bool tieneNodo(int id) const;

    int obtenerSiguienteIdDisponible() const;

    int agregarNodo(int id, QString nombre, int x, int y, QString tipo);

    int agregarNodoNuevo(QString nombre, QPointF posicion, QString tipo);

    bool eliminarNodo(int id);

    double sugerirPeso(int nodoA, int nodoB, bool esAvenida) const;

    bool agregarArista(int origen, int destino, double peso = -1.0, bool esAvenida = false);

    bool agregarCalleDoble(int nodoA, int nodoB, double peso = -1.0, bool esAvenida = false);

    bool eliminarCalle(int nodoA, int nodoB);

    bool cambiarPeso(int nodoA, int nodoB, double nuevoPeso);

    void limpiar();

    Arista *obtenerArista(int origen, int destino) const;

    bool existeAristaDirigida(int origen, int destino) const;

    bool tieneSalidas(int idNodo) const;

    double obtenerPesoEfectivo(int origen, int destino) const;

    double obtenerPesoEfectivoArista(Arista *a) const;

    double obtenerNivelCongestion(Arista *a) const;

    /** Requerimiento: El peso de cada arista aumenta por uso. */
    void registrarUso(int origen, int destino);

    int nodoMasCercano(QPointF punto, double distanciaMaxima = 1e9) const;

    void setMultiplicadorHoraPico(double multiplicador);

    double getMultiplicadorHoraPico() const;

    // ---- Algoritmo 1: Búsqueda en Anchura (BFS) usando la clase Cola ----
    ResultadoRuta buscarRutaBFS(int origen, int destino) const;

    // ---- Algoritmo 2: Búsqueda en Profundidad (DFS) usando la clase Pila ----
    ResultadoRuta buscarRutaDFS(int origen, int destino) const;

    // ---- Algoritmo 3: Algoritmo de Dijkstra (camino de menor peso ponderado) ----
    ResultadoRuta buscarRutaDijkstra(int origen, int destino) const;

    /**
     * Fórmula del Taxímetro:
     *   Taxímetro = Base ($4,600) + Distancia (salto) x $1,625 + Tiempo (ponderado) x $1,625
     *   Mínimo = $8,100
     *   Recargos: Aeropuerto (+$8,600), Terminal (+$2,400), Turístico (+$2,000), Nocturno (+$2,000)
     */
    static double calcularTarifa(int saltos, double tiempoPonderado, QString tipoDestino, QString periodo);

    static QString formatearPesos(double valor);

signals:
    void grafoModificado();
};

#endif // GRAFO_H

`

### Archivo: Grafo.cpp

`cpp
#include "Grafo.h"

#include "Arista.h"
#include "Cola.h"
#include "ElementoArista.h"
#include "ElementoNodo.h"
#include "ListaAristas.h"
#include "ListaNodos.h"
#include "Nodo.h"
#include "Pila.h"

#include <QLineF>
#include <QLocale>
#include <QtMath>
#include <cmath>

ResultadoRuta Grafo::reconstruirRuta(int anterior[], int origen, int destino, int explorados) const
{
    ResultadoRuta resultado;
    resultado.setNodosExplorados(explorados);

    if (origen == destino)
    {
        resultado.setEncontrada(true);
        resultado.agregarNodoAlCamino(origen);
        return resultado;
    }

    if (anterior[destino] == -1)
    {
        return resultado;
    }

    int inverso[100];
    int total = 0;
    int actual = destino;

    while (actual != -1 && total < 100)
    {
        inverso[total] = actual;
        total++;
        if (actual == origen)
        {
            break;
        }
        actual = anterior[actual];
    }

    if (total == 0 || inverso[total - 1] != origen)
    {
        return resultado;
    }

    double sumaPesos = 0.0;
    for (int i = total - 1; i >= 0; i--)
    {
        resultado.agregarNodoAlCamino(inverso[i]);
        if (i > 0)
        {
            sumaPesos += obtenerPesoEfectivo(inverso[i], inverso[i - 1]);
        }
    }

    resultado.setEncontrada(true);
    resultado.setTiempoPonderado(sumaPesos);
    return resultado;
}

Grafo::Grafo(QObject *parent) : QObject(parent)
{
    nodos = new ListaNodos();
    aristas = new ListaAristas();
    multiplicadorHoraPico = 1.0;
}

Grafo::~Grafo()
{
    delete nodos;
    delete aristas;
    nodos = nullptr;
    aristas = nullptr;
}

ListaNodos *Grafo::getNodos() const
{
    return nodos;
}

ListaAristas *Grafo::getAristas() const
{
    return aristas;
}

Nodo *Grafo::buscarNodo(int id) const
{
    return nodos->buscarPorId(id);
}

bool Grafo::tieneNodo(int id) const
{
    return buscarNodo(id) != nullptr;
}

int Grafo::obtenerSiguienteIdDisponible() const
{
    for (int i = 0; i < 100; i++)
    {
        if (buscarNodo(i) == nullptr)
        {
            return i;
        }
    }
    return -1;
}

int Grafo::agregarNodo(int id, QString nombre, int x, int y, QString tipo)
{
    if (id < 0 || id >= 100 || buscarNodo(id) != nullptr)
    {
        return -1;
    }

    nodos->agregar(new Nodo(id, nombre, x, y, tipo));
    emit grafoModificado();
    return id;
}

int Grafo::agregarNodoNuevo(QString nombre, QPointF posicion, QString tipo)
{
    int id = obtenerSiguienteIdDisponible();
    if (id < 0)
    {
        return -1;
    }
    return agregarNodo(id, nombre, static_cast<int>(posicion.x()), static_cast<int>(posicion.y()), tipo);
}

bool Grafo::eliminarNodo(int id)
{
    if (buscarNodo(id) == nullptr)
    {
        return false;
    }

    aristas->eliminarAristasDeNodo(id);
    bool ok = nodos->eliminarPorId(id);
    emit grafoModificado();
    return ok;
}

double Grafo::sugerirPeso(int nodoA, int nodoB, bool esAvenida) const
{
    Nodo *a = buscarNodo(nodoA);
    Nodo *b = buscarNodo(nodoB);
    if (a == nullptr || b == nullptr)
    {
        return 1.0;
    }

    double distancia = QLineF(a->getPosicion(), b->getPosicion()).length() / 130.0;
    if (esAvenida)
    {
        distancia = distancia * 0.75;
    }

    double redondeado = std::round(distancia * 10.0) / 10.0;
    if (redondeado < 0.5)
    {
        redondeado = 0.5;
    }
    return redondeado;
}

bool Grafo::agregarArista(int origen, int destino, double peso, bool esAvenida)
{
    if (origen == destino || buscarNodo(origen) == nullptr || buscarNodo(destino) == nullptr)
    {
        return false;
    }

    if (aristas->existeArista(origen, destino))
    {
        return false;
    }

    double pesoCalculado = peso;
    if (pesoCalculado <= 0.0)
    {
        pesoCalculado = sugerirPeso(origen, destino, esAvenida);
    }

    aristas->agregar(new Arista(origen, destino, pesoCalculado, esAvenida));
    emit grafoModificado();
    return true;
}

bool Grafo::agregarCalleDoble(int nodoA, int nodoB, double peso, bool esAvenida)
{
    double pesoCalculado = peso;
    if (pesoCalculado <= 0.0)
    {
        pesoCalculado = sugerirPeso(nodoA, nodoB, esAvenida);
    }

    bool ab = agregarArista(nodoA, nodoB, pesoCalculado, esAvenida);
    bool ba = agregarArista(nodoB, nodoA, pesoCalculado, esAvenida);
    return ab || ba;
}

bool Grafo::eliminarCalle(int nodoA, int nodoB)
{
    bool ab = aristas->eliminarArista(nodoA, nodoB);
    bool ba = aristas->eliminarArista(nodoB, nodoA);
    if (ab || ba)
    {
        emit grafoModificado();
        return true;
    }
    return false;
}

bool Grafo::cambiarPeso(int nodoA, int nodoB, double nuevoPeso)
{
    if (nuevoPeso <= 0.0)
    {
        return false;
    }

    Arista *ab = aristas->obtenerArista(nodoA, nodoB);
    Arista *ba = aristas->obtenerArista(nodoB, nodoA);
    if (ab == nullptr && ba == nullptr)
    {
        return false;
    }

    if (ab != nullptr)
    {
        ab->setPeso(nuevoPeso);
        ab->setPesoInicial(nuevoPeso);
    }
    if (ba != nullptr)
    {
        ba->setPeso(nuevoPeso);
        ba->setPesoInicial(nuevoPeso);
    }

    emit grafoModificado();
    return true;
}

void Grafo::limpiar()
{
    nodos->limpiar();
    aristas->limpiar();
    emit grafoModificado();
}

Arista *Grafo::obtenerArista(int origen, int destino) const
{
    return aristas->obtenerArista(origen, destino);
}

bool Grafo::existeAristaDirigida(int origen, int destino) const
{
    return aristas->existeArista(origen, destino);
}

bool Grafo::tieneSalidas(int idNodo) const
{
    ElementoArista *actual = aristas->getPrimero();
    while (actual != nullptr)
    {
        Arista *a = actual->getArista();
        if (a != nullptr && a->getOrigen() == idNodo)
        {
            return true;
        }
        actual = actual->getSiguiente();
    }
    return false;
}

double Grafo::obtenerPesoEfectivo(int origen, int destino) const
{
    Arista *a = aristas->obtenerArista(origen, destino);
    if (a == nullptr)
    {
        return 0.0;
    }
    return a->getPeso() * multiplicadorHoraPico;
}

double Grafo::obtenerPesoEfectivoArista(Arista *a) const
{
    if (a == nullptr)
    {
        return 0.0;
    }
    return a->getPeso() * multiplicadorHoraPico;
}

double Grafo::obtenerNivelCongestion(Arista *a) const
{
    if (a == nullptr)
    {
        return 1.0;
    }
    double base = a->getPesoInicial();
    if (base < 0.1)
    {
        base = 0.1;
    }
    return (a->getPeso() / base) * multiplicadorHoraPico;
}

void Grafo::registrarUso(int origen, int destino)
{
    Arista *ab = aristas->obtenerArista(origen, destino);
    if (ab == nullptr)
    {
        return;
    }

    double maximo = ab->getPesoInicial() * 3.0;
    double nuevoPeso = ab->getPeso() + 0.2;
    if (nuevoPeso > maximo)
    {
        nuevoPeso = maximo;
    }
    ab->setPeso(nuevoPeso);

    Arista *ba = aristas->obtenerArista(destino, origen);
    if (ba != nullptr)
    {
        ba->setPeso(nuevoPeso);
    }
}

int Grafo::nodoMasCercano(QPointF punto, double distanciaMaxima) const
{
    int mejorId = -1;
    double mejorDistancia = distanciaMaxima;

    ElementoNodo *actual = nodos->getPrimero();
    while (actual != nullptr)
    {
        Nodo *n = actual->getNodo();
        if (n != nullptr)
        {
            double d = QLineF(punto, n->getPosicion()).length();
            if (d <= mejorDistancia)
            {
                mejorDistancia = d;
                mejorId = n->getId();
            }
        }
        actual = actual->getSiguiente();
    }

    return mejorId;
}

void Grafo::setMultiplicadorHoraPico(double multiplicador)
{
    this->multiplicadorHoraPico = multiplicador;
}

double Grafo::getMultiplicadorHoraPico() const
{
    return multiplicadorHoraPico;
}

ResultadoRuta Grafo::buscarRutaBFS(int origen, int destino) const
{
    const int MAX = 100;
    if (origen < 0 || origen >= MAX || destino < 0 || destino >= MAX)
    {
        return ResultadoRuta();
    }
    if (buscarNodo(origen) == nullptr || buscarNodo(destino) == nullptr)
    {
        return ResultadoRuta();
    }

    bool visitado[MAX];
    int anterior[MAX];
    for (int i = 0; i < MAX; i++)
    {
        visitado[i] = false;
        anterior[i] = -1;
    }

    Cola cola;
    cola.encolar(origen);
    visitado[origen] = true;
    int explorados = 0;

    while (!cola.estaVacia())
    {
        int actual = cola.desencolar();
        explorados++;

        if (actual == destino)
        {
            break;
        }

        ElementoArista *elem = aristas->getPrimero();
        while (elem != nullptr)
        {
            Arista *a = elem->getArista();
            if (a != nullptr && a->getOrigen() == actual)
            {
                int vecino = a->getDestino();
                if (vecino >= 0 && vecino < MAX && !visitado[vecino] && buscarNodo(vecino) != nullptr)
                {
                    visitado[vecino] = true;
                    anterior[vecino] = actual;
                    cola.encolar(vecino);
                }
            }
            elem = elem->getSiguiente();
        }
    }

    return reconstruirRuta(anterior, origen, destino, explorados);
}

ResultadoRuta Grafo::buscarRutaDFS(int origen, int destino) const
{
    const int MAX = 100;
    if (origen < 0 || origen >= MAX || destino < 0 || destino >= MAX)
    {
        return ResultadoRuta();
    }
    if (buscarNodo(origen) == nullptr || buscarNodo(destino) == nullptr)
    {
        return ResultadoRuta();
    }

    bool visitado[MAX];
    int anterior[MAX];
    for (int i = 0; i < MAX; i++)
    {
        visitado[i] = false;
        anterior[i] = -1;
    }

    Pila pila;
    pila.apilar(origen);
    int explorados = 0;

    while (!pila.estaVacia())
    {
        int actual = pila.desapilar();
        if (actual < 0 || actual >= MAX || visitado[actual])
        {
            continue;
        }

        visitado[actual] = true;
        explorados++;

        if (actual == destino)
        {
            break;
        }

        ElementoArista *elem = aristas->getPrimero();
        while (elem != nullptr)
        {
            Arista *a = elem->getArista();
            if (a != nullptr && a->getOrigen() == actual)
            {
                int vecino = a->getDestino();
                if (vecino >= 0 && vecino < MAX && !visitado[vecino] && buscarNodo(vecino) != nullptr)
                {
                    if (anterior[vecino] == -1)
                    {
                        anterior[vecino] = actual;
                    }
                    pila.apilar(vecino);
                }
            }
            elem = elem->getSiguiente();
        }
    }

    return reconstruirRuta(anterior, origen, destino, explorados);
}

ResultadoRuta Grafo::buscarRutaDijkstra(int origen, int destino) const
{
    const int MAX = 100;
    if (origen < 0 || origen >= MAX || destino < 0 || destino >= MAX)
    {
        return ResultadoRuta();
    }
    if (buscarNodo(origen) == nullptr || buscarNodo(destino) == nullptr)
    {
        return ResultadoRuta();
    }

    double distancia[MAX];
    int anterior[MAX];
    bool visitado[MAX];

    for (int i = 0; i < MAX; i++)
    {
        distancia[i] = 999999.0;
        anterior[i] = -1;
        visitado[i] = false;
    }

    distancia[origen] = 0.0;
    int explorados = 0;
    int cantidad = nodos->getCantidad();

    for (int i = 0; i < cantidad; i++)
    {
        int actual = -1;
        double menorDistancia = 999999.0;

        for (int j = 0; j < MAX; j++)
        {
            if (!visitado[j] && buscarNodo(j) != nullptr && distancia[j] < menorDistancia)
            {
                menorDistancia = distancia[j];
                actual = j;
            }
        }

        if (actual == -1)
        {
            break;
        }

        visitado[actual] = true;
        explorados++;

        if (actual == destino)
        {
            break;
        }

        ElementoArista *elem = aristas->getPrimero();
        while (elem != nullptr)
        {
            Arista *a = elem->getArista();
            if (a != nullptr && a->getOrigen() == actual)
            {
                int vecino = a->getDestino();
                if (vecino >= 0 && vecino < MAX && !visitado[vecino])
                {
                    double nuevoCosto = distancia[actual] + obtenerPesoEfectivoArista(a);
                    if (nuevoCosto < distancia[vecino])
                    {
                        distancia[vecino] = nuevoCosto;
                        anterior[vecino] = actual;
                    }
                }
            }
            elem = elem->getSiguiente();
        }
    }

    return reconstruirRuta(anterior, origen, destino, explorados);
}

double Grafo::calcularTarifa(int saltos, double tiempoPonderado, QString tipoDestino, QString periodo)
{
    double base = 4600.0;
    double costoDistancia = saltos * 1625.0;
    double costoTiempo = tiempoPonderado * 1625.0;
    double taximetro = base + costoDistancia + costoTiempo;

    if (taximetro < 8100.0)
    {
        taximetro = 8100.0;
    }

    double recargoDestino = 0.0;
    if (tipoDestino == "Aeropuerto")
    {
        recargoDestino = 8600.0;
    }
    else if (tipoDestino == "Terminal")
    {
        recargoDestino = 2400.0;
    }
    else if (tipoDestino == "Turistico")
    {
        recargoDestino = 2000.0;
    }

    double recargoNocturno = 0.0;
    if (periodo == "Nocturno")
    {
        recargoNocturno = 2000.0;
    }

    return taximetro + recargoDestino + recargoNocturno;
}

QString Grafo::formatearPesos(double valor)
{
    qint64 redondeado = qRound64(valor);
    return QStringLiteral("$ ") + QLocale(QLocale::English).toString(qAbs(redondeado));
}

`

### Archivo: ResultadoRuta.h

`cpp
#ifndef RESULTADO_RUTA_H
#define RESULTADO_RUTA_H

class ResultadoRuta
{
private:
    bool encontrada;
    int camino[100];
    int cantidadNodos;
    double tiempoPonderado;
    int nodosExplorados;

public:
    ResultadoRuta();

    bool getEncontrada() const;
    int getCantidadNodos() const;
    int getNodoEn(int indice) const;
    double getTiempoPonderado() const;
    int getNodosExplorados() const;

    int getSaltos() const;

    void setEncontrada(bool encontrada);
    void setTiempoPonderado(double tiempoPonderado);
    void setNodosExplorados(int nodosExplorados);

    void limpiarCamino();

    void agregarNodoAlCamino(int idNodo);
};

#endif // RESULTADO_RUTA_H

`

### Archivo: ResultadoRuta.cpp

`cpp
#include "ResultadoRuta.h"

ResultadoRuta::ResultadoRuta()
{
    encontrada = false;
    cantidadNodos = 0;
    tiempoPonderado = 0.0;
    nodosExplorados = 0;
}

bool ResultadoRuta::getEncontrada() const
{
    return encontrada;
}

int ResultadoRuta::getCantidadNodos() const
{
    return cantidadNodos;
}

int ResultadoRuta::getNodoEn(int indice) const
{
    return camino[indice];
}

double ResultadoRuta::getTiempoPonderado() const
{
    return tiempoPonderado;
}

int ResultadoRuta::getNodosExplorados() const
{
    return nodosExplorados;
}

int ResultadoRuta::getSaltos() const
{
    if (cantidadNodos <= 1)
    {
        return 0;
    }
    return cantidadNodos - 1;
}

void ResultadoRuta::setEncontrada(bool encontrada)
{
    this->encontrada = encontrada;
}

void ResultadoRuta::setTiempoPonderado(double tiempoPonderado)
{
    this->tiempoPonderado = tiempoPonderado;
}

void ResultadoRuta::setNodosExplorados(int nodosExplorados)
{
    this->nodosExplorados = nodosExplorados;
}

void ResultadoRuta::limpiarCamino()
{
    cantidadNodos = 0;
}

void ResultadoRuta::agregarNodoAlCamino(int idNodo)
{
    if (cantidadNodos < 100)
    {
        camino[cantidadNodos] = idNodo;
        cantidadNodos++;
    }
}

`


---

## 4. Simulación, Entidades y Tipos de Taxis

### Archivo: Simulacion.h

`cpp
#ifndef SIMULACION_H
#define SIMULACION_H

#include <QColor>
#include <QElapsedTimer>
#include <QObject>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>
#include <QString>
#include <QTimer>
#include <QVector>

#include "ManzanaCiudad.h"
#include "Pasajero.h"
#include "RelojSimulacion.h"

class Grafo;
class Taxi;
class TaxiManual;

class Simulacion : public QObject
{
    Q_OBJECT

private:
    Grafo *grafo;
    RelojSimulacion reloj;
    QRectF limitesMapa;
    QPolygonF agua;
    QVector<ManzanaCiudad> manzanas;

    Taxi *taxis[7];
    int cantidadTaxis;
    TaxiManual *taxiManual;

    QVector<Pasajero> pasajeros;
    int siguienteIdPasajero;

    QTimer temporizador;
    QElapsedTimer relojCuadro;
    bool pausada;
    double velocidad;
    double tiempoSimulado;
    double acumuladorMuestra;
    double cuentaRegresivaPasajero;
    QString ultimoPeriodo;

    void agregarManzana(int n1, int n2, int n3, int n4, QString tipoManzana);

    void construirCiudadInicial();

    void liberarTaxis();

    void crearTaxis();

    void generarPasajero();

    int buscarIndicePasajeroPorId(int idPasajero) const;

    int asignarPasajeroATaxi(Taxi *taxi);

    void revisarParadaTaxi(Taxi *taxi);

    void planificarRutaAutonoma(Taxi *taxi);

    void actualizarTaxi(Taxi *taxi, double dt);

    void avanzarUnPaso(double dt);

    void actualizarPeriodo(bool silencioso);

    QString nombreNodo(int id) const;

    void registrarEvento(QString texto, QColor color);

private slots:
    void enTickReloj();

public:
    explicit Simulacion(QObject *parent = nullptr);

    ~Simulacion() override;

    Grafo *getGrafo() const;
    const RelojSimulacion &getReloj() const;
    int getCantidadTaxis() const;
    Taxi *getTaxi(int indice) const;
    TaxiManual *getTaxiManual() const;
    const QVector<Pasajero> &getPasajerosEnEspera() const;
    QRectF getLimitesMapa() const;
    QPolygonF getPoligonoAgua() const;
    const QVector<ManzanaCiudad> &getManzanas() const;

    void iniciar();

    bool estaPausada() const;

    void setPausada(bool p);

    void setVelocidad(double mult);

    void setModoTiempo(int modo);

    void reiniciar();

    double cotizarCarrera(int origen, int destino) const;

signals:
    void cuadroActualizado(double dtReal);
    void muestraGrafico(double segundosSimulados);
    void reinicioCompletado();
    void pausaCambiada(bool pausada);
    void mensajeEvento(QString texto, QColor color);
    void carreraCompletada(int idTaxi, QPointF posMapa, double tarifa, QColor color);
};

#endif // SIMULACION_H

`

### Archivo: Simulacion.cpp

`cpp
#include "Simulacion.h"

#include "ElementoNodo.h"
#include "Grafo.h"
#include "ListaNodos.h"
#include "Nodo.h"
#include "ResultadoRuta.h"
#include "Taxi.h"
#include "TaxiBFS.h"
#include "TaxiDFS.h"
#include "TaxiDijkstra.h"
#include "TaxiManual.h"

#include <QRandomGenerator>
#include <cmath>

void Simulacion::agregarManzana(int n1, int n2, int n3, int n4, QString tipoManzana)
{
    QVector<QPointF> pts;
    if (n1 >= 0 && grafo->buscarNodo(n1) != nullptr) pts.append(grafo->buscarNodo(n1)->getPosicion());
    if (n2 >= 0 && grafo->buscarNodo(n2) != nullptr) pts.append(grafo->buscarNodo(n2)->getPosicion());
    if (n3 >= 0 && grafo->buscarNodo(n3) != nullptr) pts.append(grafo->buscarNodo(n3)->getPosicion());
    if (n4 >= 0 && grafo->buscarNodo(n4) != nullptr) pts.append(grafo->buscarNodo(n4)->getPosicion());

    QPointF centro(0, 0);
    for (int i = 0; i < pts.size(); i++)
    {
        centro += pts[i];
    }
    centro /= pts.size();

    QPolygonF poli;
    for (int i = 0; i < pts.size(); i++)
    {
        poli << centro + (pts[i] - centro) * 0.78;
    }
    manzanas.append(ManzanaCiudad(poli, tipoManzana));
}

void Simulacion::construirCiudadInicial()
{
    grafo->limpiar();
    limitesMapa = QRectF(0, 0, 1600, 1080);

    grafo->agregarNodo(0,  "Puerto Norte",        260,  130, "Normal");
    grafo->agregarNodo(1,  "Mirador del Cerro",   520,  110, "Turistico");
    grafo->agregarNodo(2,  "Parque Norte",        800,  130, "Normal");
    grafo->agregarNodo(3,  "Distrito Financiero", 1080, 120, "Normal");
    grafo->agregarNodo(4,  "Aeropuerto",          1400, 130, "Aeropuerto");
    grafo->agregarNodo(5,  "Barrio Alto",         200,  330, "Normal");
    grafo->agregarNodo(6,  "Plaza Mayor",         470,  320, "Normal");
    grafo->agregarNodo(7,  "Zona Rosa",           760,  330, "Normal");
    grafo->agregarNodo(8,  "Centro",              1030, 340, "Normal");
    grafo->agregarNodo(9,  "Universidad",         1260, 330, "Normal");
    grafo->agregarNodo(10, "Zona Industrial",     1500, 340, "Normal");
    grafo->agregarNodo(11, "Bosque",              180,  540, "Normal");
    grafo->agregarNodo(12, "Museo Nacional",      430,  530, "Turistico");
    grafo->agregarNodo(13, "Av. Libertad",        700,  550, "Normal");
    grafo->agregarNodo(14, "Ciudad Vieja",        960,  560, "Normal");
    grafo->agregarNodo(15, "Hospital",            1200, 550, "Normal");
    grafo->agregarNodo(16, "Centro Comercial",    1440, 560, "Normal");
    grafo->agregarNodo(17, "Muelle Turístico",    170,  770, "Turistico");
    grafo->agregarNodo(18, "Barrio Sur",          420,  750, "Normal");
    grafo->agregarNodo(19, "Parque del Río",      680,  770, "Normal");
    grafo->agregarNodo(20, "Gran Estadio",        950,  780, "Turistico");
    grafo->agregarNodo(21, "Zona Franca",         1210, 770, "Normal");
    grafo->agregarNodo(22, "Terminal Central",    1470, 780, "Terminal");
    grafo->agregarNodo(23, "Bahía",               300,  940, "Normal");
    grafo->agregarNodo(24, "Puente Sur",          600,  950, "Normal");
    grafo->agregarNodo(25, "Mercado",             860,  960, "Normal");
    grafo->agregarNodo(26, "Barrio Obrero",       1120, 950, "Normal");
    grafo->agregarNodo(27, "Terminal Sur",        1380, 950, "Terminal");

    grafo->agregarCalleDoble(0, 1, -1.0, false);
    grafo->agregarCalleDoble(1, 2, -1.0, false);
    grafo->agregarCalleDoble(2, 3, -1.0, false);
    grafo->agregarCalleDoble(3, 4, -1.0, true);
    grafo->agregarCalleDoble(5, 6, -1.0, true);
    grafo->agregarCalleDoble(6, 7, -1.0, true);
    grafo->agregarCalleDoble(7, 8, -1.0, true);
    grafo->agregarCalleDoble(8, 9, -1.0, true);
    grafo->agregarCalleDoble(9, 10, -1.0, true);
    grafo->agregarCalleDoble(11, 12, -1.0, false);
    grafo->agregarCalleDoble(12, 13, -1.0, false);
    grafo->agregarCalleDoble(13, 14, -1.0, false);
    grafo->agregarCalleDoble(14, 15, -1.0, false);
    grafo->agregarCalleDoble(15, 16, -1.0, false);
    grafo->agregarCalleDoble(17, 18, -1.0, false);
    grafo->agregarCalleDoble(18, 19, -1.0, false);
    grafo->agregarCalleDoble(19, 20, -1.0, false);
    grafo->agregarCalleDoble(20, 21, -1.0, false);
    grafo->agregarCalleDoble(21, 22, -1.0, false);
    grafo->agregarCalleDoble(23, 24, -1.0, false);
    grafo->agregarCalleDoble(24, 25, -1.0, false);
    grafo->agregarCalleDoble(25, 26, -1.0, false);
    grafo->agregarCalleDoble(26, 27, -1.0, false);

    grafo->agregarCalleDoble(0, 5, -1.0, false);
    grafo->agregarCalleDoble(5, 11, -1.0, false);
    grafo->agregarCalleDoble(11, 17, -1.0, false);
    grafo->agregarCalleDoble(17, 23, -1.0, false);
    grafo->agregarCalleDoble(1, 6, -1.0, false);
    grafo->agregarCalleDoble(6, 12, -1.0, false);
    grafo->agregarCalleDoble(12, 18, -1.0, false);
    grafo->agregarCalleDoble(18, 23, -1.0, false);
    grafo->agregarCalleDoble(2, 7, -1.0, false);
    grafo->agregarCalleDoble(7, 13, -1.0, false);
    grafo->agregarCalleDoble(13, 19, -1.0, false);
    grafo->agregarCalleDoble(19, 24, -1.0, false);
    grafo->agregarCalleDoble(3, 8, -1.0, true);
    grafo->agregarCalleDoble(8, 14, -1.0, true);
    grafo->agregarCalleDoble(14, 20, -1.0, true);
    grafo->agregarCalleDoble(20, 25, -1.0, true);
    grafo->agregarCalleDoble(4, 9, -1.0, false);
    grafo->agregarCalleDoble(9, 15, -1.0, false);
    grafo->agregarCalleDoble(15, 21, -1.0, false);
    grafo->agregarCalleDoble(21, 26, -1.0, false);
    grafo->agregarCalleDoble(10, 16, -1.0, false);
    grafo->agregarCalleDoble(16, 22, -1.0, false);
    grafo->agregarCalleDoble(22, 27, -1.0, false);

    agua.clear();
    agua << QPointF(0, 0) << QPointF(105, 0) << QPointF(62, 190) << QPointF(98, 420)
         << QPointF(55, 640) << QPointF(100, 850) << QPointF(180, 1010) << QPointF(250, 1080)
         << QPointF(0, 1080);

    manzanas.clear();
    agregarManzana(0, 1, 6, 5, "Edificios");
    agregarManzana(5, 6, 12, 11, "Parque");
    agregarManzana(11, 12, 18, 17, "Edificios");
    agregarManzana(17, 18, 23, -1, "Edificios");
    agregarManzana(1, 2, 7, 6, "Edificios");
    agregarManzana(6, 7, 13, 12, "Edificios");
    agregarManzana(12, 13, 19, 18, "Edificios");
    agregarManzana(18, 19, 24, 23, "Parque");
    agregarManzana(2, 3, 8, 7, "Edificios");
    agregarManzana(7, 8, 14, 13, "Parque");
    agregarManzana(13, 14, 20, 19, "Edificios");
    agregarManzana(19, 20, 25, 24, "Edificios");
    agregarManzana(3, 4, 9, 8, "Aeropuerto");
    agregarManzana(8, 9, 15, 14, "Edificios");
    agregarManzana(14, 15, 21, 20, "Edificios");
    agregarManzana(20, 21, 26, 25, "Edificios");
    agregarManzana(9, 10, 16, 15, "Industrial");
    agregarManzana(15, 16, 22, 21, "Edificios");
    agregarManzana(21, 22, 27, 26, "Terminal");
}

void Simulacion::liberarTaxis()
{
    for (int i = 0; i < cantidadTaxis; i++)
    {
        delete taxis[i];
        taxis[i] = nullptr;
    }
    cantidadTaxis = 0;
    taxiManual = nullptr;
}

void Simulacion::crearTaxis()
{
    liberarTaxis();

    int nodosIniciales[7] = {0, 2, 5, 9, 12, 20, 8};
    for (int i = 0; i < 7; i++)
    {
        int j = QRandomGenerator::global()->bounded(7);
        int temp = nodosIniciales[i];
        nodosIniciales[i] = nodosIniciales[j];
        nodosIniciales[j] = temp;
    }

    taxis[0] = new TaxiBFS(0, "BFS 1", QColor("#4cc9f0"), nodosIniciales[0], grafo);
    taxis[1] = new TaxiBFS(1, "BFS 2", QColor("#3a86ff"), nodosIniciales[1], grafo);
    taxis[2] = new TaxiDFS(2, "DFS 1", QColor("#ff5d8f"), nodosIniciales[2], grafo);
    taxis[3] = new TaxiDFS(3, "DFS 2", QColor("#c9184a"), nodosIniciales[3], grafo);
    taxis[4] = new TaxiDijkstra(4, "Dijkstra 1", QColor("#80ed99"), nodosIniciales[4], grafo);
    taxis[5] = new TaxiDijkstra(5, "Dijkstra 2", QColor("#2dc653"), nodosIniciales[5], grafo);

    taxiManual = new TaxiManual(6, "Tú", QColor("#ffd60a"), nodosIniciales[6], grafo);
    taxis[6] = taxiManual;
    cantidadTaxis = 7;
}

void Simulacion::generarPasajero()
{
    int candidatos[100];
    int totalCandidatos = 0;

    ElementoNodo *elem = grafo->getNodos()->getPrimero();
    while (elem != nullptr)
    {
        Nodo *n = elem->getNodo();
        if (n != nullptr && grafo->tieneSalidas(n->getId()) && totalCandidatos < 100)
        {
            candidatos[totalCandidatos] = n->getId();
            totalCandidatos++;
        }
        elem = elem->getSiguiente();
    }

    if (totalCandidatos < 2)
    {
        return;
    }

    QRandomGenerator &rng = *QRandomGenerator::global();

    for (int intento = 0; intento < 8; intento++)
    {
        int origen = candidatos[rng.bounded(totalCandidatos)];

        bool ocupado = false;
        for (int i = 0; i < pasajeros.size(); i++)
        {
            if (pasajeros[i].getOrigen() == origen)
            {
                ocupado = true;
            }
        }
        if (ocupado)
        {
            continue;
        }

        double probabilidades[100];
        double sumaPesos = 0.0;
        for (int i = 0; i < totalCandidatos; i++)
        {
            int id = candidatos[i];
            double prob = 1.0;
            if (id == origen)
            {
                prob = 0.0;
            }
            else if (grafo->buscarNodo(id)->getTipo() == "Turistico")
            {
                prob = 3.0; // Mayor probabilidad de ser destino de un usuario
            }
            probabilidades[i] = prob;
            sumaPesos += prob;
        }

        double eleccion = rng.generateDouble() * sumaPesos;
        int destino = -1;
        for (int i = 0; i < totalCandidatos; i++)
        {
            eleccion -= probabilidades[i];
            if (eleccion <= 0.0 && probabilidades[i] > 0.0)
            {
                destino = candidatos[i];
                break;
            }
        }

        if (destino < 0 || !grafo->buscarRutaDijkstra(origen, destino).getEncontrada())
        {
            continue;
        }

        double tarifa = cotizarCarrera(origen, destino);
        pasajeros.append(Pasajero(siguienteIdPasajero++, origen, destino, tarifa));
        return;
    }
}

int Simulacion::buscarIndicePasajeroPorId(int idPasajero) const
{
    for (int i = 0; i < pasajeros.size(); i++)
    {
        if (pasajeros[i].getId() == idPasajero)
        {
            return i;
        }
    }
    return -1;
}

int Simulacion::asignarPasajeroATaxi(Taxi *taxi)
{
    int mejorIdx = -1;
    double mejorCosto = 999999.0;

    for (int i = 0; i < pasajeros.size(); i++)
    {
        if (pasajeros[i].estaReservado())
        {
            continue;
        }
        ResultadoRuta r = taxi->calcularRuta(grafo, taxi->getNodoActual(), pasajeros[i].getOrigen());
        if (r.getEncontrada() && r.getTiempoPonderado() < mejorCosto)
        {
            mejorCosto = r.getTiempoPonderado();
            mejorIdx = i;
        }
    }

    if (mejorIdx >= 0)
    {
        pasajeros[mejorIdx].setIdTaxiAsignado(taxi->getId());
        return pasajeros[mejorIdx].getId();
    }
    return -1;
}

void Simulacion::revisarParadaTaxi(Taxi *taxi)
{
    if (taxi->getLlevaPasajero() && taxi->getNodoActual() == taxi->getDestinoPasajeroABordo())
    {
        int nodoDest = taxi->getDestinoPasajeroABordo();
        double tarifa = taxi->getTarifaPasajeroABordo();
        taxi->bajarPasajero();

        emit carreraCompletada(taxi->getId(), taxi->getPosicion(), tarifa, taxi->getColor());
        registrarEvento(QStringLiteral("%1 (%2) completó carrera en %3: +%4")
                            .arg(taxi->getNombre(), taxi->nombreAlgoritmo(),
                                 nombreNodo(nodoDest), Grafo::formatearPesos(tarifa)),
                        taxi->getColor());
    }

    if (!taxi->getLlevaPasajero())
    {
        for (int i = 0; i < pasajeros.size(); i++)
        {
            const Pasajero &p = pasajeros[i];
            if (p.getOrigen() == taxi->getNodoActual()
                && (!p.estaReservado() || p.getIdTaxiAsignado() == taxi->getId()))
            {
                int origen = p.getOrigen();
                int destino = p.getDestino();
                double tarifa = cotizarCarrera(origen, destino);
                pasajeros.removeAt(i);

                taxi->subirPasajero(destino, tarifa);
                registrarEvento(
                    QStringLiteral("%1 (%2) recogió pasajero en %3 → %4 (tarifa %5)")
                        .arg(taxi->getNombre(), taxi->nombreAlgoritmo(),
                             nombreNodo(origen), nombreNodo(destino),
                             Grafo::formatearPesos(tarifa)),
                    taxi->getColor());
                break;
            }
        }
    }
}

void Simulacion::planificarRutaAutonoma(Taxi *taxi)
{
    if (taxi->esManual())
    {
        return;
    }

    if (taxi->getLlevaPasajero())
    {
        ResultadoRuta res = taxi->calcularRuta(grafo, taxi->getNodoActual(),
                                               taxi->getDestinoPasajeroABordo());
        if (res.getEncontrada())
        {
            taxi->cargarRutaDesdeResultado(res);
        }
        return;
    }

    int idx = buscarIndicePasajeroPorId(taxi->getIdPasajeroObjetivo());
    if (idx < 0)
    {
        taxi->setIdPasajeroObjetivo(asignarPasajeroATaxi(taxi));
        idx = buscarIndicePasajeroPorId(taxi->getIdPasajeroObjetivo());
        if (idx < 0)
        {
            return;
        }
    }

    int nodoOrigenPasajero = pasajeros[idx].getOrigen();
    if (taxi->getNodoActual() != nodoOrigenPasajero)
    {
        ResultadoRuta res = taxi->calcularRuta(grafo, taxi->getNodoActual(), nodoOrigenPasajero);
        if (res.getEncontrada())
        {
            taxi->cargarRutaDesdeResultado(res);
        }
        else
        {
            pasajeros[idx].setIdTaxiAsignado(-1);
            taxi->setIdPasajeroObjetivo(-1);
        }
    }
}

void Simulacion::actualizarTaxi(Taxi *taxi, double dt)
{
    taxi->suavizarAngulo(dt);

    if (taxi->getEnMovimiento())
    {
        if (taxi->avanzarEnTramo(dt, grafo))
        {
            revisarParadaTaxi(taxi);
        }
        return;
    }

    if (!grafo->tieneNodo(taxi->getNodoActual()))
    {
        int cercano = grafo->nodoMasCercano(taxi->getPosicion());
        if (cercano >= 0)
        {
            taxi->setNodoActual(cercano);
            taxi->setPosicion(grafo->buscarNodo(cercano)->getPosicion());
            taxi->limpiarRuta();
        }
    }

    if (taxi->getCantidadRuta() == 0)
    {
        taxi->setEsperaPensar(taxi->getEsperaPensar() - dt);
        if (taxi->getEsperaPensar() > 0.0)
        {
            return;
        }

        revisarParadaTaxi(taxi);
        if (taxi->getCantidadRuta() == 0)
        {
            planificarRutaAutonoma(taxi);
        }
        if (taxi->getCantidadRuta() == 0)
        {
            revisarParadaTaxi(taxi);
        }
        if (taxi->getCantidadRuta() == 0)
        {
            taxi->setEsperaPensar(0.2);
            return;
        }
    }

    taxi->iniciarTramo(grafo);
}

void Simulacion::avanzarUnPaso(double dt)
{
    tiempoSimulado += dt;
    reloj.avanzar(dt);
    actualizarPeriodo(false);

    cuentaRegresivaPasajero -= dt;
    if (cuentaRegresivaPasajero <= 0.0)
    {
        if (pasajeros.size() < 14)
        {
            generarPasajero();
        }
        cuentaRegresivaPasajero = 1.2 + QRandomGenerator::global()->generateDouble() * 1.4;
    }

    for (int i = 0; i < cantidadTaxis; i++)
    {
        actualizarTaxi(taxis[i], dt);
    }

    for (int i = pasajeros.size() - 1; i >= 0; i--)
    {
        if (!grafo->tieneNodo(pasajeros[i].getOrigen()) || !grafo->tieneNodo(pasajeros[i].getDestino()))
        {
            pasajeros.removeAt(i);
        }
    }

    acumuladorMuestra += dt;
    while (acumuladorMuestra >= 1.0)
    {
        acumuladorMuestra -= 1.0;
        emit muestraGrafico(tiempoSimulado);
    }
}

void Simulacion::actualizarPeriodo(bool silencioso)
{
    QString p = reloj.periodoActual();
    if (p == ultimoPeriodo && !silencioso)
    {
        return;
    }
    ultimoPeriodo = p;
    grafo->setMultiplicadorHoraPico(p == "HoraPico" ? 1.8 : 1.0);
    if (silencioso)
    {
        return;
    }

    if (p == "HoraPico")
    {
        registrarEvento(QStringLiteral("HORA PICO: ponderaciones mayores en las aristas (×1.8)."),
                        QColor(255, 99, 99));
    }
    else if (p == "Nocturno")
    {
        registrarEvento(QStringLiteral("MODO NOCTURNO: recargo de +$ 2,000 por carrera."),
                        QColor(167, 139, 250));
    }
    else
    {
        registrarEvento(QStringLiteral("Modo normal activado."), QColor(74, 222, 128));
    }
}

QString Simulacion::nombreNodo(int id) const
{
    Nodo *n = grafo->buscarNodo(id);
    return (n != nullptr) ? n->getNombre() : QStringLiteral("?");
}

void Simulacion::registrarEvento(QString texto, QColor color)
{
    emit mensajeEvento(QStringLiteral("[%1] %2").arg(reloj.textoHora(), texto), color);
}

void Simulacion::enTickReloj()
{
    double dtReal = relojCuadro.restart() / 1000.0;
    if (dtReal > 0.1)
    {
        dtReal = 0.1;
    }
    if (!pausada)
    {
        double totalDt = dtReal * velocidad;
        int pasos = static_cast<int>(std::ceil(totalDt / 0.05));
        if (pasos < 1)
        {
            pasos = 1;
        }
        for (int i = 0; i < pasos; i++)
        {
            avanzarUnPaso(totalDt / pasos);
        }
    }
    emit cuadroActualizado(dtReal);
}

Simulacion::Simulacion(QObject *parent) : QObject(parent)
{
    grafo = new Grafo(this);
    for (int i = 0; i < 7; i++)
    {
        taxis[i] = nullptr;
    }
    cantidadTaxis = 0;
    taxiManual = nullptr;
    siguienteIdPasajero = 1;
    pausada = false;
    velocidad = 1.0;
    tiempoSimulado = 0.0;
    acumuladorMuestra = 0.0;
    cuentaRegresivaPasajero = 0.0;
    ultimoPeriodo = "Normal";

    construirCiudadInicial();
    crearTaxis();
    actualizarPeriodo(true);
    for (int i = 0; i < 6; i++)
    {
        generarPasajero();
    }

    temporizador.setInterval(16);
    connect(&temporizador, &QTimer::timeout, this, &Simulacion::enTickReloj);
}

Simulacion::~Simulacion()
{
    liberarTaxis();
}

Grafo *Simulacion::getGrafo() const
{
    return grafo;
}

const RelojSimulacion &Simulacion::getReloj() const
{
    return reloj;
}

int Simulacion::getCantidadTaxis() const
{
    return cantidadTaxis;
}

Taxi *Simulacion::getTaxi(int indice) const
{
    return taxis[indice];
}

TaxiManual *Simulacion::getTaxiManual() const
{
    return taxiManual;
}

const QVector<Pasajero> &Simulacion::getPasajerosEnEspera() const
{
    return pasajeros;
}

QRectF Simulacion::getLimitesMapa() const
{
    return limitesMapa;
}

QPolygonF Simulacion::getPoligonoAgua() const
{
    return agua;
}

const QVector<ManzanaCiudad> &Simulacion::getManzanas() const
{
    return manzanas;
}

void Simulacion::iniciar()
{
    relojCuadro.start();
    temporizador.start();
}

bool Simulacion::estaPausada() const
{
    return pausada;
}

void Simulacion::setPausada(bool p)
{
    if (pausada == p)
    {
        return;
    }
    pausada = p;
    emit pausaCambiada(pausada);
}

void Simulacion::setVelocidad(double mult)
{
    velocidad = mult;
}

void Simulacion::setModoTiempo(int modo)
{
    reloj.setModo(modo);
    actualizarPeriodo(false);
}

void Simulacion::reiniciar()
{
    construirCiudadInicial();
    pasajeros.clear();
    siguienteIdPasajero = 1;
    tiempoSimulado = 0.0;
    acumuladorMuestra = 0.0;
    reloj.reiniciar();
    crearTaxis();
    actualizarPeriodo(true);
    for (int i = 0; i < 6; i++)
    {
        generarPasajero();
    }
    emit reinicioCompletado();
    registrarEvento(QStringLiteral("Simulación reiniciada."), QColor(250, 204, 21));
}

double Simulacion::cotizarCarrera(int origen, int destino) const
{
    ResultadoRuta r = grafo->buscarRutaDijkstra(origen, destino);
    Nodo *nodoDest = grafo->buscarNodo(destino);
    QString tipoDest = (nodoDest != nullptr) ? nodoDest->getTipo() : QStringLiteral("Normal");
    return Grafo::calcularTarifa(r.getSaltos(), r.getTiempoPonderado(), tipoDest, reloj.periodoActual());
}

`

### Archivo: Taxi.h

`cpp
#ifndef TAXI_H
#define TAXI_H

#include <QColor>
#include <QPointF>
#include <QString>

#include "ResultadoRuta.h"

class Grafo;

class Taxi
{
protected:
    int id;
    QString nombre;
    QColor color;
    int nodoActual;
    QPointF posicion;
    double angulo;
    double anguloObjetivo;

    bool enMovimiento;
    int nodoSiguiente;
    QPointF inicioTramo;
    QPointF finTramo;
    double progresoTramo;
    double duracionTramo;
    double esperaPensar;

    int ruta[100];
    int cantidadRuta;

    bool llevaPasajero;
    int destinoPasajeroABordo;
    double tarifaPasajeroABordo;
    int idPasajeroObjetivo;

    int viajes;
    double ingresos;
    long nodosExplorados;

public:
    Taxi(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    virtual ~Taxi();

    virtual QString nombreAlgoritmo() const = 0;
    virtual bool esManual() const;
    virtual ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino);

    int getId() const;
    QString getNombre() const;
    QColor getColor() const;
    int getNodoActual() const;
    QPointF getPosicion() const;
    double getAngulo() const;
    bool getEnMovimiento() const;
    int getNodoSiguiente() const;
    int getCantidadRuta() const;
    int getPasoRuta(int indice) const;
    bool getLlevaPasajero() const;
    int getDestinoPasajeroABordo() const;
    double getTarifaPasajeroABordo() const;
    int getIdPasajeroObjetivo() const;
    int getViajes() const;
    double getIngresos() const;
    long getNodosExplorados() const;
    double getEsperaPensar() const;

    void setNodoActual(int idNodo);
    void setPosicion(QPointF pos);
    void setIdPasajeroObjetivo(int idPasajero);
    void setEsperaPensar(double espera);
    void sumarExplorados(int cantidad);

    void subirPasajero(int destino, double tarifa);

    void bajarPasajero();

    void limpiarRuta();

    void agregarPasoARuta(int idNodo);

    void cargarRutaDesdeResultado(const ResultadoRuta &res);

    int quitarPrimerPasoRuta();

    int nodoFinalDeRuta() const;

    void iniciarTramo(Grafo *grafo);

    bool avanzarEnTramo(double dt, Grafo *grafo);

    void suavizarAngulo(double dt);

    QString textoEstado(Grafo *grafo) const;
};

#endif // TAXI_H

`

### Archivo: Taxi.cpp

`cpp
#include "Taxi.h"

#include "Grafo.h"
#include "Nodo.h"

#include <QtMath>
#include <cmath>

Taxi::Taxi(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
{
    this->id = id;
    this->nombre = nombre;
    this->color = color;
    this->nodoActual = nodoInicial;
    this->angulo = 0.0;
    this->anguloObjetivo = 0.0;
    this->enMovimiento = false;
    this->nodoSiguiente = -1;
    this->progresoTramo = 0.0;
    this->duracionTramo = 1.0;
    this->esperaPensar = 0.0;
    this->cantidadRuta = 0;
    this->llevaPasajero = false;
    this->destinoPasajeroABordo = -1;
    this->tarifaPasajeroABordo = 0.0;
    this->idPasajeroObjetivo = -1;
    this->viajes = 0;
    this->ingresos = 0.0;
    this->nodosExplorados = 0;

    Nodo *n = (grafo != nullptr) ? grafo->buscarNodo(nodoInicial) : nullptr;
    if (n != nullptr)
    {
        this->posicion = n->getPosicion();
    }
}

Taxi::~Taxi()
{
}

bool Taxi::esManual() const
{
    return false;
}

ResultadoRuta Taxi::calcularRuta(Grafo *grafo, int origen, int destino)
{
    Q_UNUSED(grafo);
    Q_UNUSED(origen);
    Q_UNUSED(destino);
    return ResultadoRuta();
}

int Taxi::getId() const
{
    return id;
}

QString Taxi::getNombre() const
{
    return nombre;
}

QColor Taxi::getColor() const
{
    return color;
}

int Taxi::getNodoActual() const
{
    return nodoActual;
}

QPointF Taxi::getPosicion() const
{
    return posicion;
}

double Taxi::getAngulo() const
{
    return angulo;
}

bool Taxi::getEnMovimiento() const
{
    return enMovimiento;
}

int Taxi::getNodoSiguiente() const
{
    return nodoSiguiente;
}

int Taxi::getCantidadRuta() const
{
    return cantidadRuta;
}

int Taxi::getPasoRuta(int indice) const
{
    return ruta[indice];
}

bool Taxi::getLlevaPasajero() const
{
    return llevaPasajero;
}

int Taxi::getDestinoPasajeroABordo() const
{
    return destinoPasajeroABordo;
}

double Taxi::getTarifaPasajeroABordo() const
{
    return tarifaPasajeroABordo;
}

int Taxi::getIdPasajeroObjetivo() const
{
    return idPasajeroObjetivo;
}

int Taxi::getViajes() const
{
    return viajes;
}

double Taxi::getIngresos() const
{
    return ingresos;
}

long Taxi::getNodosExplorados() const
{
    return nodosExplorados;
}

double Taxi::getEsperaPensar() const
{
    return esperaPensar;
}

void Taxi::setNodoActual(int idNodo)
{
    this->nodoActual = idNodo;
}

void Taxi::setPosicion(QPointF pos)
{
    this->posicion = pos;
}

void Taxi::setIdPasajeroObjetivo(int idPasajero)
{
    this->idPasajeroObjetivo = idPasajero;
}

void Taxi::setEsperaPensar(double espera)
{
    this->esperaPensar = espera;
}

void Taxi::sumarExplorados(int cantidad)
{
    this->nodosExplorados += cantidad;
}

void Taxi::subirPasajero(int destino, double tarifa)
{
    llevaPasajero = true;
    destinoPasajeroABordo = destino;
    tarifaPasajeroABordo = tarifa;
    idPasajeroObjetivo = -1;
}

void Taxi::bajarPasajero()
{
    ingresos += tarifaPasajeroABordo;
    viajes++;
    llevaPasajero = false;
    destinoPasajeroABordo = -1;
    tarifaPasajeroABordo = 0.0;
    idPasajeroObjetivo = -1;
}

void Taxi::limpiarRuta()
{
    cantidadRuta = 0;
}

void Taxi::agregarPasoARuta(int idNodo)
{
    if (cantidadRuta < 100)
    {
        ruta[cantidadRuta] = idNodo;
        cantidadRuta++;
    }
}

void Taxi::cargarRutaDesdeResultado(const ResultadoRuta &res)
{
    limpiarRuta();
    for (int i = 1; i < res.getCantidadNodos(); i++)
    {
        agregarPasoARuta(res.getNodoEn(i));
    }
}

int Taxi::quitarPrimerPasoRuta()
{
    if (cantidadRuta <= 0)
    {
        return -1;
    }
    int primero = ruta[0];
    for (int i = 0; i < cantidadRuta - 1; i++)
    {
        ruta[i] = ruta[i + 1];
    }
    cantidadRuta--;
    return primero;
}

int Taxi::nodoFinalDeRuta() const
{
    if (cantidadRuta > 0)
    {
        return ruta[cantidadRuta - 1];
    }
    if (enMovimiento)
    {
        return nodoSiguiente;
    }
    return nodoActual;
}

void Taxi::iniciarTramo(Grafo *grafo)
{
    if (grafo == nullptr || cantidadRuta <= 0)
    {
        return;
    }

    int siguiente = ruta[0];
    if (!grafo->existeAristaDirigida(nodoActual, siguiente))
    {
        limpiarRuta();
        esperaPensar = 0.0;
        return;
    }

    double peso = grafo->obtenerPesoEfectivo(nodoActual, siguiente);
    duracionTramo = peso * 0.75;
    if (duracionTramo < 0.2)
    {
        duracionTramo = 0.2;
    }

    grafo->registrarUso(nodoActual, siguiente);

    inicioTramo = grafo->buscarNodo(nodoActual)->getPosicion();
    finTramo = grafo->buscarNodo(siguiente)->getPosicion();
    nodoSiguiente = siguiente;
    progresoTramo = 0.0;
    enMovimiento = true;
    quitarPrimerPasoRuta();

    QPointF d = finTramo - inicioTramo;
    anguloObjetivo = qRadiansToDegrees(std::atan2(d.y(), d.x()));
}

bool Taxi::avanzarEnTramo(double dt, Grafo *grafo)
{
    progresoTramo += dt / duracionTramo;
    if (progresoTramo < 1.0)
    {
        posicion = inicioTramo + (finTramo - inicioTramo) * progresoTramo;
        return false;
    }

    enMovimiento = false;
    nodoActual = nodoSiguiente;
    Nodo *n = (grafo != nullptr) ? grafo->buscarNodo(nodoActual) : nullptr;
    posicion = (n != nullptr) ? n->getPosicion() : finTramo;
    esperaPensar = 0.0;
    return true;
}

void Taxi::suavizarAngulo(double dt)
{
    double dif = std::fmod(anguloObjetivo - angulo + 540.0, 360.0) - 180.0;
    double factor = dt * 10.0;
    if (factor > 1.0)
    {
        factor = 1.0;
    }
    angulo += dif * factor;
}

QString Taxi::textoEstado(Grafo *grafo) const
{
    if (llevaPasajero)
    {
        Nodo *dest = (grafo != nullptr) ? grafo->buscarNodo(destinoPasajeroABordo) : nullptr;
        QString nombreDest = (dest != nullptr) ? dest->getNombre() : QStringLiteral("?");
        return QStringLiteral("Llevando a %1  ·  tarifa %2")
            .arg(nombreDest, Grafo::formatearPesos(tarifaPasajeroABordo));
    }
    if (esManual())
    {
        return QStringLiteral("Libre: haz clic en una calle conectada");
    }
    return QStringLiteral("Buscando pasajero");
}

`

### Archivo: TaxiBFS.h

`cpp
#ifndef TAXI_BFS_H
#define TAXI_BFS_H

#include "Taxi.h"

class TaxiBFS : public Taxi
{
public:
    TaxiBFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    QString nombreAlgoritmo() const override;

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override;
};

#endif // TAXI_BFS_H

`

### Archivo: TaxiBFS.cpp

`cpp
#include "TaxiBFS.h"

#include "Grafo.h"

TaxiBFS::TaxiBFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
    : Taxi(id, nombre, color, nodoInicial, grafo)
{
}

QString TaxiBFS::nombreAlgoritmo() const
{
    return QStringLiteral("BFS");
}

ResultadoRuta TaxiBFS::calcularRuta(Grafo *grafo, int origen, int destino)
{
    ResultadoRuta res = grafo->buscarRutaBFS(origen, destino);
    sumarExplorados(res.getNodosExplorados());
    return res;
}

`

### Archivo: TaxiDFS.h

`cpp
#ifndef TAXI_DFS_H
#define TAXI_DFS_H

#include "Taxi.h"

class TaxiDFS : public Taxi
{
public:
    TaxiDFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    QString nombreAlgoritmo() const override;

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override;
};

#endif // TAXI_DFS_H

`

### Archivo: TaxiDFS.cpp

`cpp
#include "TaxiDFS.h"

#include "Grafo.h"

TaxiDFS::TaxiDFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
    : Taxi(id, nombre, color, nodoInicial, grafo)
{
}

QString TaxiDFS::nombreAlgoritmo() const
{
    return QStringLiteral("DFS");
}

ResultadoRuta TaxiDFS::calcularRuta(Grafo *grafo, int origen, int destino)
{
    ResultadoRuta res = grafo->buscarRutaDFS(origen, destino);
    sumarExplorados(res.getNodosExplorados());
    return res;
}

`

### Archivo: TaxiDijkstra.h

`cpp
#ifndef TAXI_DIJKSTRA_H
#define TAXI_DIJKSTRA_H

#include "Taxi.h"

class TaxiDijkstra : public Taxi
{
public:
    TaxiDijkstra(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    QString nombreAlgoritmo() const override;

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override;
};

#endif // TAXI_DIJKSTRA_H

`

### Archivo: TaxiDijkstra.cpp

`cpp
#include "TaxiDijkstra.h"

#include "Grafo.h"

TaxiDijkstra::TaxiDijkstra(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
    : Taxi(id, nombre, color, nodoInicial, grafo)
{
}

QString TaxiDijkstra::nombreAlgoritmo() const
{
    return QStringLiteral("Dijkstra");
}

ResultadoRuta TaxiDijkstra::calcularRuta(Grafo *grafo, int origen, int destino)
{
    ResultadoRuta res = grafo->buscarRutaDijkstra(origen, destino);
    sumarExplorados(res.getNodosExplorados());
    return res;
}

`

### Archivo: TaxiManual.h

`cpp
#ifndef TAXI_MANUAL_H
#define TAXI_MANUAL_H

#include "Taxi.h"

class TaxiManual : public Taxi
{
public:
    TaxiManual(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    QString nombreAlgoritmo() const override;

    bool esManual() const override;

    bool agregarNodoARuta(int idNodo, Grafo *grafo);
};

#endif // TAXI_MANUAL_H

`

### Archivo: TaxiManual.cpp

`cpp
#include "TaxiManual.h"

#include "Grafo.h"

TaxiManual::TaxiManual(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
    : Taxi(id, nombre, color, nodoInicial, grafo)
{
}

QString TaxiManual::nombreAlgoritmo() const
{
    return QStringLiteral("Manual");
}

bool TaxiManual::esManual() const
{
    return true;
}

bool TaxiManual::agregarNodoARuta(int idNodo, Grafo *grafo)
{
    int desde = nodoFinalDeRuta();
    if (grafo == nullptr || !grafo->existeAristaDirigida(desde, idNodo))
    {
        return false;
    }
    agregarPasoARuta(idNodo);
    return true;
}

`

### Archivo: Pasajero.h

`cpp
#ifndef PASAJERO_H
#define PASAJERO_H

class Pasajero
{
private:
    int id;
    int origen;
    int destino;
    int idTaxiAsignado;
    double tarifa;

public:
    Pasajero(int id, int origen, int destino, double tarifa);

    int getId() const;
    int getOrigen() const;
    int getDestino() const;
    int getIdTaxiAsignado() const;
    double getTarifa() const;
    bool estaReservado() const;

    void setIdTaxiAsignado(int idTaxi);
    void setTarifa(double tarifa);
};

#endif // PASAJERO_H

`

### Archivo: Pasajero.cpp

`cpp
#include "Pasajero.h"

Pasajero::Pasajero(int id, int origen, int destino, double tarifa)
{
    this->id = id;
    this->origen = origen;
    this->destino = destino;
    this->idTaxiAsignado = -1;
    this->tarifa = tarifa;
}

int Pasajero::getId() const
{
    return id;
}

int Pasajero::getOrigen() const
{
    return origen;
}

int Pasajero::getDestino() const
{
    return destino;
}

int Pasajero::getIdTaxiAsignado() const
{
    return idTaxiAsignado;
}

double Pasajero::getTarifa() const
{
    return tarifa;
}

bool Pasajero::estaReservado() const
{
    return idTaxiAsignado >= 0;
}

void Pasajero::setIdTaxiAsignado(int idTaxi)
{
    this->idTaxiAsignado = idTaxi;
}

void Pasajero::setTarifa(double tarifa)
{
    this->tarifa = tarifa;
}

`

### Archivo: RelojSimulacion.h

`cpp
#ifndef RELOJ_SIMULACION_H
#define RELOJ_SIMULACION_H

#include <QString>

class RelojSimulacion
{
private:
    double hora;
    int dia;
    int modo; // 0 = Automático, 1 = Normal, 2 = Nocturno, 3 = Hora Pico

public:
    RelojSimulacion();

    void reiniciar();

    void avanzar(double segundosSimulados);

    int getDia() const;
    int getModo() const;
    void setModo(int modo);

    QString textoHora() const;

    QString periodoActual() const;

    double oscuridad() const;
};

#endif // RELOJ_SIMULACION_H

`

### Archivo: RelojSimulacion.cpp

`cpp
#include "RelojSimulacion.h"

RelojSimulacion::RelojSimulacion()
{
    hora = 6.0;
    dia = 1;
    modo = 0;
}

void RelojSimulacion::reiniciar()
{
    hora = 6.0;
    dia = 1;
}

void RelojSimulacion::avanzar(double segundosSimulados)
{
    hora += segundosSimulados * (24.0 / 240.0);
    while (hora >= 24.0)
    {
        hora -= 24.0;
        dia++;
    }
}

int RelojSimulacion::getDia() const
{
    return dia;
}

int RelojSimulacion::getModo() const
{
    return modo;
}

void RelojSimulacion::setModo(int modo)
{
    this->modo = modo;
}

QString RelojSimulacion::textoHora() const
{
    int h = static_cast<int>(hora);
    int m = static_cast<int>((hora - h) * 60.0);
    return QStringLiteral("%1:%2")
        .arg(h, 2, 10, QLatin1Char('0'))
        .arg(m, 2, 10, QLatin1Char('0'));
}

QString RelojSimulacion::periodoActual() const
{
    if (modo == 1)
    {
        return "Normal";
    }
    if (modo == 2)
    {
        return "Nocturno";
    }
    if (modo == 3)
    {
        return "HoraPico";
    }

    if ((hora >= 6.5 && hora < 9.0) || (hora >= 11.5 && hora < 14.0) || (hora >= 17.0 && hora < 19.5))
    {
        return "HoraPico";
    }
    if (hora >= 21.0 || hora < 5.0)
    {
        return "Nocturno";
    }

    return "Normal";
}

double RelojSimulacion::oscuridad() const
{
    if (modo == 2)
    {
        return 1.0;
    }
    if (modo == 1 || modo == 3)
    {
        return 0.0;
    }

    if (hora >= 21.5 || hora < 5.0)
    {
        return 1.0;
    }
    if (hora >= 19.0)
    {
        return (hora - 19.0) / 2.5;
    }
    if (hora < 7.0)
    {
        return 1.0 - (hora - 5.0) / 2.0;
    }
    return 0.0;
}

`

### Archivo: ManzanaCiudad.h

`cpp
#ifndef MANZANA_CIUDAD_H
#define MANZANA_CIUDAD_H

#include <QPolygonF>
#include <QString>

class ManzanaCiudad
{
private:
    QPolygonF poligono;
    QString tipo;

public:
    ManzanaCiudad(QPolygonF poligono, QString tipo);

    QPolygonF getPoligono() const;
    QString getTipo() const;
};

#endif // MANZANA_CIUDAD_H

`

### Archivo: ManzanaCiudad.cpp

`cpp
#include "ManzanaCiudad.h"

ManzanaCiudad::ManzanaCiudad(QPolygonF poligono, QString tipo)
{
    this->poligono = poligono;
    this->tipo = tipo;
}

QPolygonF ManzanaCiudad::getPoligono() const
{
    return poligono;
}

QString ManzanaCiudad::getTipo() const
{
    return tipo;
}

`

### Archivo: TextoFlotante.h

`cpp
#ifndef TEXTO_FLOTANTE_H
#define TEXTO_FLOTANTE_H

#include <QColor>
#include <QPointF>
#include <QString>

class TextoFlotante
{
private:
    QPointF posicion;
    QString texto;
    QColor color;
    double edad;

public:
    TextoFlotante(QPointF posicion, QString texto, QColor color);

    QPointF getPosicion() const;
    QString getTexto() const;
    QColor getColor() const;
    double getEdad() const;
    void avanzarEdad(double dt);
};

#endif // TEXTO_FLOTANTE_H

`

### Archivo: TextoFlotante.cpp

`cpp
#include "TextoFlotante.h"

TextoFlotante::TextoFlotante(QPointF posicion, QString texto, QColor color)
{
    this->posicion = posicion;
    this->texto = texto;
    this->color = color;
    this->edad = 0.0;
}

QPointF TextoFlotante::getPosicion() const
{
    return posicion;
}

QString TextoFlotante::getTexto() const
{
    return texto;
}

QColor TextoFlotante::getColor() const
{
    return color;
}

double TextoFlotante::getEdad() const
{
    return edad;
}

void TextoFlotante::avanzarEdad(double dt)
{
    edad += dt;
}

`


---

## 5. Interfaz Gráfica de Usuario (GUI, Vistas y Diálogos)

### Archivo: MainWindow.h

`cpp
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QColor>
#include <QMainWindow>
#include <QPointF>
#include <QString>
#include <QTimer>

class BarrasGananciasWidget;
class GraficoGananciasWidget;
class MapaWidget;
class QTableWidget;
class Simulacion;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    Simulacion *sim;
    MapaWidget *mapaWidget;
    BarrasGananciasWidget *barrasWidget;
    GraficoGananciasWidget *graficoWidget;

    /*
        Modos de edición:
        0 = conducir taxi manual
        1 = agregar nodo
        2 = agregar calle
        3 = eliminar calle
        4 = eliminar nodo
        5 = cambiar peso de calle
    */
    int modoEdicion;
    int nodoSeleccionadoEdicion;
    QTimer temporizadorTablas;

    static void asignarCelda(QTableWidget *tabla, int fila, int col, QString texto,
                             QColor color, Qt::Alignment alineacion);

    void actualizarTextoAyudaEdicion();

    void mostrarMensajeEstado(QString texto);

    void configurarPanelGuia();
    void actualizarResumenGrafo();
    void actualizarEstadoSimulacion();

private slots:
    void enPausaCambiada(bool pausada);

    void actualizarBotonPausa(bool pausada);

    void enMostrarPesosCambiado(bool activo);

    void enVelocidadCambiada(int indice);

    void enModoTiempoCambiado(int indice);

    void enModoEdicionCambiado(int indice);

    void enReiniciarClicado();

    void enCancelarClicado();

    void enEspacioVacioClicado(QPointF posMapa);

    void enNodoClicado(int idNodo);

    void agregarEvento(QString texto, QColor color);

    void configurarTablas();

    void actualizarTablas();

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;
};

#endif // MAIN_WINDOW_H

`

### Archivo: MainWindow.cpp

`cpp
#include "MainWindow.h"

#include "Arista.h"
#include "BarrasGananciasWidget.h"
#include "DialogoNodo.h"
#include "GraficoGananciasWidget.h"
#include "Grafo.h"
#include "ListaAristas.h"
#include "ListaNodos.h"
#include "MapaWidget.h"
#include "Nodo.h"
#include "Pasajero.h"
#include "RelojSimulacion.h"
#include "Simulacion.h"
#include "Taxi.h"
#include "TaxiManual.h"
#include "ui_mainwindow.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFontMetrics>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVector>
#include <QWidget>

namespace
{
QPixmap marcaGrafo()
{
    QPixmap imagen(64, 64);
    imagen.setDevicePixelRatio(2.0);
    imagen.fill(Qt::transparent);

    QPainter p(&imagen);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#3d4f60"), 1));
    p.setBrush(QColor("#202e3a"));
    p.drawRoundedRect(QRectF(1.5, 1.5, 29, 29), 4, 4);

    const QPointF origen(8.5, 19.5);
    const QPointF norte(13.5, 9.5);
    const QPointF cruce(19.5, 16.5);
    const QPointF destino(24, 23);
    p.setPen(QPen(QColor("#93a9be"), 1.3, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(origen, norte);
    p.drawLine(origen, cruce);
    p.drawLine(norte, cruce);
    p.drawLine(cruce, destino);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#cbd9e5"));
    p.drawEllipse(origen, 2, 2);
    p.drawEllipse(norte, 2, 2);
    p.drawEllipse(cruce, 2, 2);
    p.setBrush(QColor("#ffd166"));
    p.drawEllipse(destino, 2.5, 2.5);
    return imagen;
}
}

void MainWindow::asignarCelda(QTableWidget *tabla, int fila, int col, QString texto,
                              QColor color, Qt::Alignment alineacion)
{
    QTableWidgetItem *item = tabla->item(fila, col);
    if (item == nullptr)
    {
        item = new QTableWidgetItem();
        tabla->setItem(fila, col, item);
    }
    item->setText(texto);
    item->setForeground(color);
    item->setTextAlignment(alineacion | Qt::AlignVCenter);
}

void MainWindow::actualizarTextoAyudaEdicion()
{
    QString texto;
    if (modoEdicion == 0)
        texto = QStringLiteral("Pulsa un nodo resaltado para conducir. Clic derecho para cancelar.");
    else if (modoEdicion == 1)
        texto = QStringLiteral("Pulsa un espacio libre para ubicar el nodo.");
    else if (modoEdicion == 2)
        texto = QStringLiteral("Selecciona el nodo de origen y después el de destino.");
    else if (modoEdicion == 3)
        texto = QStringLiteral("Selecciona los dos extremos de la calle.");
    else if (modoEdicion == 4)
        texto = QStringLiteral("Selecciona el nodo que quieres eliminar junto con sus calles.");
    else if (modoEdicion == 5)
        texto = QStringLiteral("Selecciona los dos extremos de la calle para cambiar su peso.");
    ui->lblEditHint->setText(texto);
}

void MainWindow::mostrarMensajeEstado(QString texto)
{
    ui->statusbar->showMessage(texto, 8000);
    ui->lblEditHint->setText(texto);
}

void MainWindow::configurarPanelGuia()
{
    ui->txtInfo->setHtml(QStringLiteral(R"(
<h3 style="color:#f4f7fa">Cómo funciona TaxiGraph</h3>
<ul>
<li><b>Ciudad y Grafo:</b> Grafo dirigido y ponderado (<code>class Grafo</code> con <code>ListaNodos</code> y <code>ListaAristas</code>) donde las calles iniciales son de doble sentido (dos aristas dirigidas A→B y B→A) y es modificable por interfaz y por código.</li>
<li><b>Taxis:</b> 6 autónomos (2 con <b>BFS</b>, 2 con <b>DFS</b>, 2 con <b>Dijkstra</b>) y 1 controlado por el usuario (<b>Tú</b>).</li>
<li><b>Fórmula del Taxímetro (Mín. $8,100):</b><br>
<code>Taxímetro = Base ($4,600) + Distancia (saltos) × $1,625 + Tiempo (ponderado) × $1,625</code></li>
<li><b>Destinos Especiales:</b> Aeropuerto (+$8,600), Terminal (+$2,400), Sitios turísticos (+$2,000 y triple probabilidad de ser destino).</li>
<li><b>Tráfico por uso:</b> Cada vez que un taxi cruza una arista, el peso de esa arista aumenta (+0.2).</li>
<li><b>Modos:</b> Nocturno (+$2,000 de recargo) y Hora Pico (ponderaciones ×1.8).</li>
</ul>
)"));
}

void MainWindow::enPausaCambiada(bool pausada)
{
    sim->setPausada(pausada);
}

void MainWindow::actualizarBotonPausa(bool pausada)
{
    ui->btnPause->blockSignals(true);
    ui->btnPause->setChecked(pausada);
    ui->btnPause->setText(pausada ? QStringLiteral("Reanudar") : QStringLiteral("Pausar"));
    ui->btnPause->blockSignals(false);
}

void MainWindow::enMostrarPesosCambiado(bool activo)
{
    mapaWidget->setMostrarPesos(activo);
}

void MainWindow::enVelocidadCambiada(int indice)
{
    if (indice == 1) sim->setVelocidad(2.0);
    else if (indice == 2) sim->setVelocidad(4.0);
    else sim->setVelocidad(1.0);
}

void MainWindow::enModoTiempoCambiado(int indice)
{
    sim->setModoTiempo(indice);
}

void MainWindow::enModoEdicionCambiado(int indice)
{
    modoEdicion = indice;
    nodoSeleccionadoEdicion = -1;
    mapaWidget->setNodoPendiente(-1);
    mapaWidget->setModoConducir(modoEdicion == 0);
    actualizarTextoAyudaEdicion();
}

void MainWindow::enReiniciarClicado()
{
    bool estabaPausada = sim->estaPausada();
    sim->setPausada(true);
    if (QMessageBox::question(this, QStringLiteral("Reiniciar"),
                              QStringLiteral("¿Deseas reiniciar los taxis, ingresos y el mapa original?"))
        == QMessageBox::Yes)
    {
        sim->reiniciar();
    }
    sim->setPausada(estabaPausada);
}

void MainWindow::enCancelarClicado()
{
    if (modoEdicion == 0 && sim->getTaxiManual() != nullptr)
    {
        sim->getTaxiManual()->limpiarRuta();
        mostrarMensajeEstado(QStringLiteral("Ruta manual cancelada."));
    }
    else
    {
        nodoSeleccionadoEdicion = -1;
        mapaWidget->setNodoPendiente(-1);
    }
}

void MainWindow::enEspacioVacioClicado(QPointF posMapa)
{
    if (modoEdicion != 1)
    {
        return;
    }

    bool estabaPausada = sim->estaPausada();
    sim->setPausada(true);
    DialogoNodo dialogo(this);
    if (dialogo.exec() == QDialog::Accepted)
    {
        QString nombre = dialogo.getNombreNodo();
        if (nombre.isEmpty())
        {
            nombre = QStringLiteral("Nuevo nodo");
        }
        sim->getGrafo()->agregarNodoNuevo(nombre, posMapa, dialogo.getTipoNodo());
        mostrarMensajeEstado(
            QStringLiteral("Nodo \"%1\" creado. Conéctalo usando \"Agregar calle\".").arg(nombre));
    }
    sim->setPausada(estabaPausada);
}

void MainWindow::enNodoClicado(int idNodo)
{
    Grafo *g = sim->getGrafo();
    Nodo *nodoClic = g->buscarNodo(idNodo);
    if (nodoClic == nullptr)
    {
        return;
    }

    if (modoEdicion == 0)
    {
        if (sim->getTaxiManual() != nullptr && !sim->getTaxiManual()->agregarNodoARuta(idNodo, g))
        {
            mostrarMensajeEstado(
                QStringLiteral("No hay una calle directa hacia \"%1\".").arg(nodoClic->getNombre()));
        }
        return;
    }

    if (modoEdicion == 1)
    {
        return;
    }

    if (modoEdicion == 4)
    {
        bool estabaPausada = sim->estaPausada();
        sim->setPausada(true);
        QString nombre = nodoClic->getNombre();
        if (QMessageBox::question(this, QStringLiteral("Eliminar nodo"),
                                  QStringLiteral("¿Eliminar \"%1\" y sus calles?").arg(nombre))
            == QMessageBox::Yes)
        {
            g->eliminarNodo(idNodo);
            mostrarMensajeEstado(QStringLiteral("Nodo \"%1\" eliminado.").arg(nombre));
        }
        sim->setPausada(estabaPausada);
        return;
    }

    if (nodoSeleccionadoEdicion < 0)
    {
        nodoSeleccionadoEdicion = idNodo;
        mapaWidget->setNodoPendiente(idNodo);
        return;
    }

    int origen = nodoSeleccionadoEdicion;
    int destino = idNodo;
    nodoSeleccionadoEdicion = -1;
    mapaWidget->setNodoPendiente(-1);
    if (origen == destino || g->buscarNodo(origen) == nullptr)
    {
        return;
    }

    bool estabaPausada = sim->estaPausada();
    sim->setPausada(true);
    QString nombreA = g->buscarNodo(origen)->getNombre();
    QString nombreB = g->buscarNodo(destino)->getNombre();

    if (modoEdicion == 2)
    {
        QMessageBox cuadro(QMessageBox::Question, QStringLiteral("Nueva calle"),
                           QStringLiteral("¿Cómo deseas conectar \"%1\" y \"%2\"?").arg(nombreA, nombreB),
                           QMessageBox::NoButton, this);
        QAbstractButton *btnDoble = cuadro.addButton(QStringLiteral("Doble sentido"), QMessageBox::AcceptRole);
        QAbstractButton *btnUnico = cuadro.addButton(
            QStringLiteral("Un sentido (%1 → %2)").arg(nombreA, nombreB), QMessageBox::AcceptRole);
        cuadro.addButton(QStringLiteral("Cancelar"), QMessageBox::RejectRole);
        cuadro.exec();

        if (cuadro.clickedButton() == btnDoble)
        {
            g->agregarCalleDoble(origen, destino);
            mostrarMensajeEstado(
                QStringLiteral("Calle de doble sentido creada entre \"%1\" y \"%2\".").arg(nombreA, nombreB));
        }
        else if (cuadro.clickedButton() == btnUnico)
        {
            g->agregarArista(origen, destino);
            mostrarMensajeEstado(QStringLiteral("Arista dirigida creada: %1 → %2.").arg(nombreA, nombreB));
        }
    }
    else if (modoEdicion == 3)
    {
        bool ok = g->eliminarCalle(origen, destino);
        mostrarMensajeEstado(ok ? QStringLiteral("Calle eliminada entre \"%1\" y \"%2\".").arg(nombreA, nombreB)
                                : QStringLiteral("No existe calle entre \"%1\" y \"%2\".").arg(nombreA, nombreB));
    }
    else if (modoEdicion == 5)
    {
        Arista *arista = g->obtenerArista(origen, destino);
        if (arista == nullptr)
        {
            mostrarMensajeEstado(QStringLiteral("No existe calle entre \"%1\" y \"%2\".").arg(nombreA, nombreB));
        }
        else
        {
            bool ok = false;
            double nuevoPeso = QInputDialog::getDouble(
                this, QStringLiteral("Cambiar peso"),
                QStringLiteral("Nuevo peso para la calle entre \"%1\" y \"%2\":").arg(nombreA, nombreB),
                arista->getPeso(), 0.1, 50.0, 1, &ok);
            if (ok)
            {
                g->cambiarPeso(origen, destino, nuevoPeso);
                mostrarMensajeEstado(QStringLiteral("Peso actualizado a %1.").arg(nuevoPeso));
            }
        }
    }

    sim->setPausada(estabaPausada);
}

void MainWindow::agregarEvento(QString texto, QColor color)
{
    ui->txtLog->append(QStringLiteral("<span style=\"color:%1\">%2</span>")
                           .arg(color.name(), texto.toHtmlEscaped()));
}

void MainWindow::configurarTablas()
{
    QTableWidget *t = ui->tableTaxis;
    t->clear();
    t->setColumnCount(5);
    t->setRowCount(sim->getCantidadTaxis());
    t->setHorizontalHeaderLabels({QStringLiteral("Taxi"), QStringLiteral("Algoritmo"),
                                  QStringLiteral("Viajes"), QStringLiteral("Ingresos"),
                                  QStringLiteral("Nodos")});
    t->horizontalHeaderItem(4)->setToolTip(QStringLiteral("Nodos explorados al calcular rutas"));
    t->verticalHeader()->setDefaultSectionSize(27);
    t->verticalHeader()->setVisible(false);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionMode(QAbstractItemView::NoSelection);
    t->setAlternatingRowColors(true);
    t->setShowGrid(false);
    t->setWordWrap(false);
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    actualizarTablas();
}

void MainWindow::actualizarResumenGrafo()
{
    const Grafo *grafo = sim->getGrafo();
    ui->lblGraphCounts->setText(QStringLiteral("%1 nodos · %2 aristas")
                                    .arg(grafo->getNodos()->getCantidad())
                                    .arg(grafo->getAristas()->getCantidad()));
}

void MainWindow::actualizarEstadoSimulacion()
{
    const RelojSimulacion &reloj = sim->getReloj();
    const QString periodo = reloj.periodoActual();
    const QString estado = periodo == "HoraPico" ? QStringLiteral("Hora pico ×1.8")
                         : periodo == "Nocturno" ? QStringLiteral("Nocturno +$2.000")
                                                : QStringLiteral("Tráfico normal");
    ui->lblGraphClock->setText(reloj.textoHora());
    ui->lblGraphPeriod->setText(QStringLiteral("Día %1 · %2").arg(reloj.getDia()).arg(estado));

    const TaxiManual *taxi = sim->getTaxiManual();
    ui->lblManualTrips->setText(QStringLiteral("%1 viajes").arg(taxi->getViajes()));
    ui->lblManualIncome->setText(Grafo::formatearPesos(taxi->getIngresos()));
    const QString contexto = taxi->getLlevaPasajero()
        ? taxi->textoEstado(sim->getGrafo())
        : QStringLiteral("Disponible");
    ui->lblManualState->setText(ui->lblManualState->fontMetrics().elidedText(
        contexto, Qt::ElideRight, ui->lblManualState->width()));
    ui->lblManualState->setToolTip(contexto);
}

void MainWindow::actualizarTablas()
{
    actualizarEstadoSimulacion();
    QTableWidget *t = ui->tableTaxis;
    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *tx = sim->getTaxi(i);
        asignarCelda(t, i, 0, QStringLiteral("● ") + tx->getNombre(), tx->getColor(), Qt::AlignLeft);
        asignarCelda(t, i, 1, tx->nombreAlgoritmo(), QColor("#dbe3ef"), Qt::AlignLeft);
        asignarCelda(t, i, 2, QString::number(tx->getViajes()), QColor("#dbe3ef"), Qt::AlignRight);
        asignarCelda(t, i, 3, Grafo::formatearPesos(tx->getIngresos()), QColor("#89e3be"), Qt::AlignRight);
        asignarCelda(t, i, 4,
                     tx->esManual() ? QStringLiteral("-") : QString::number(tx->getNodosExplorados()),
                     QColor("#dbe3ef"), Qt::AlignRight);
    }

    QTableWidget *tp = ui->tablePassengers;
    if (tp->columnCount() != 4)
    {
        tp->setColumnCount(4);
        tp->setHorizontalHeaderLabels({QStringLiteral("Estado"), QStringLiteral("Origen"),
                                       QStringLiteral("Destino"), QStringLiteral("Tarifa")});
        tp->verticalHeader()->setDefaultSectionSize(30);
        tp->verticalHeader()->setVisible(false);
        tp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tp->setSelectionMode(QAbstractItemView::NoSelection);
        tp->setAlternatingRowColors(true);
        tp->setShowGrid(false);
        tp->setWordWrap(false);
        tp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }

    const QVector<Pasajero> &lista = sim->getPasajerosEnEspera();
    tp->setRowCount(lista.size());
    Grafo *g = sim->getGrafo();
    int libres = 0;

    for (int i = 0; i < lista.size(); i++)
    {
        const Pasajero &p = lista[i];
        QString estado = QStringLiteral("● Libre");
        QColor colorEstado("#5eead4");

        if (p.estaReservado())
        {
            for (int k = 0; k < sim->getCantidadTaxis(); k++)
            {
                Taxi *tx = sim->getTaxi(k);
                if (tx->getId() == p.getIdTaxiAsignado())
                {
                    estado = QStringLiteral("→ ") + tx->getNombre();
                    colorEstado = tx->getColor();
                }
            }
        }
        else
        {
            libres++;
        }

        Nodo *no = g->buscarNodo(p.getOrigen());
        Nodo *nd = g->buscarNodo(p.getDestino());
        asignarCelda(tp, i, 0, estado, colorEstado, Qt::AlignLeft);
        asignarCelda(tp, i, 1, (no != nullptr) ? no->getNombre() : QStringLiteral("?"),
                     QColor("#dbe3ef"), Qt::AlignLeft);
        asignarCelda(tp, i, 2, (nd != nullptr) ? nd->getNombre() : QStringLiteral("?"),
                     QColor("#dbe3ef"), Qt::AlignLeft);
        asignarCelda(tp, i, 3,
                     Grafo::formatearPesos(sim->cotizarCarrera(p.getOrigen(), p.getDestino())),
                     QColor("#ffd60a"), Qt::AlignRight);
    }

    ui->tabs->setTabText(1, QStringLiteral("Pasajeros · %1").arg(libres));
    ui->tabs->setTabToolTip(1, QStringLiteral("%1 pasajeros libres").arg(libres));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui = new Ui::MainWindow();
    sim = new Simulacion(this);
    mapaWidget = nullptr;
    barrasWidget = nullptr;
    graficoWidget = nullptr;
    modoEdicion = 0;
    nodoSeleccionadoEdicion = -1;

    ui->setupUi(this);
    const QPixmap marca = marcaGrafo();
    ui->lblGraphMark->setPixmap(marca);
    setWindowIcon(QIcon(marca));
    ui->lblNodeFocus->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->layoutGraphLegend->setStretch(4, 1);

    mapaWidget = new MapaWidget(ui->mapHost);
    ui->layoutMapHost->insertWidget(2, mapaWidget, 1);
    barrasWidget = new BarrasGananciasWidget(ui->barsHost);
    ui->barsHost->layout()->addWidget(barrasWidget);
    graficoWidget = new GraficoGananciasWidget(ui->chartHost);
    ui->chartHost->layout()->addWidget(graficoWidget);

    mapaWidget->setSimulacion(sim);
    mapaWidget->setMostrarPesos(ui->chkWeights->isChecked());
    barrasWidget->setSimulacion(sim);
    graficoWidget->setSimulacion(sim);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);
    ui->splitter->setSizes({920, 420});

    configurarTablas();
    configurarPanelGuia();
    actualizarTextoAyudaEdicion();
    actualizarResumenGrafo();

    connect(sim->getGrafo(), &Grafo::grafoModificado, this, &MainWindow::actualizarResumenGrafo);
    connect(mapaWidget, &MapaWidget::nodoEnFocoCambiado, this, [this](int id) {
        const Nodo *nodo = sim->getGrafo()->buscarNodo(id);
        const QString detalle = nodo != nullptr
            ? QStringLiteral("Nodo %1 · %2").arg(id).arg(nodo->getNombre())
            : QStringLiteral("Pasa el cursor por un nodo");
        ui->lblNodeFocus->setText(ui->lblNodeFocus->fontMetrics().elidedText(
            detalle, Qt::ElideRight, ui->lblNodeFocus->width()));
        ui->lblNodeFocus->setToolTip(detalle);
    });
    connect(ui->btnPause, &QPushButton::toggled, this, &MainWindow::enPausaCambiada);
    connect(ui->btnReset, &QPushButton::clicked, this, &MainWindow::enReiniciarClicado);
    connect(ui->comboSpeed, &QComboBox::currentIndexChanged, this, &MainWindow::enVelocidadCambiada);
    connect(ui->comboTimeMode, &QComboBox::currentIndexChanged, this, &MainWindow::enModoTiempoCambiado);
    connect(ui->comboEditMode, &QComboBox::currentIndexChanged, this, &MainWindow::enModoEdicionCambiado);
    connect(ui->chkWeights, &QCheckBox::toggled, this, &MainWindow::enMostrarPesosCambiado);

    connect(mapaWidget, &MapaWidget::nodoClicado, this, &MainWindow::enNodoClicado);
    connect(mapaWidget, &MapaWidget::espacioVacioClicado, this, &MainWindow::enEspacioVacioClicado);
    connect(mapaWidget, &MapaWidget::cancelarSolicitado, this, &MainWindow::enCancelarClicado);

    connect(sim, &Simulacion::mensajeEvento, this, &MainWindow::agregarEvento);
    connect(sim, &Simulacion::reinicioCompletado, this, &MainWindow::configurarTablas);
    connect(sim, &Simulacion::pausaCambiada, this, &MainWindow::actualizarBotonPausa);

    temporizadorTablas.setInterval(250);
    connect(&temporizadorTablas, &QTimer::timeout, this, &MainWindow::actualizarTablas);
    temporizadorTablas.start();

    agregarEvento(
        QStringLiteral("Simulación iniciada."),
        QColor("#ffd60a"));
    sim->iniciar();
}

MainWindow::~MainWindow()
{
    delete ui;
    ui = nullptr;
}

`

### Archivo: mainwindow.ui

`xml
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>1360</width>
    <height>800</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>TaxiGraph</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <layout class="QVBoxLayout" name="layoutMain">
    <property name="spacing">
     <number>10</number>
    </property>
    <property name="leftMargin">
     <number>12</number>
    </property>
    <property name="topMargin">
     <number>12</number>
    </property>
    <property name="rightMargin">
     <number>12</number>
    </property>
    <property name="bottomMargin">
     <number>8</number>
    </property>
    <item>
     <widget class="QWidget" name="appHeader">
      <property name="sizePolicy">
       <sizepolicy hsizetype="Expanding" vsizetype="Fixed">
        <horstretch>0</horstretch>
        <verstretch>0</verstretch>
       </sizepolicy>
      </property>
      <layout class="QHBoxLayout" name="layoutAppHeader">
       <property name="spacing">
        <number>16</number>
       </property>
       <property name="leftMargin">
        <number>14</number>
       </property>
       <property name="topMargin">
        <number>4</number>
       </property>
       <property name="rightMargin">
        <number>12</number>
       </property>
       <property name="bottomMargin">
        <number>4</number>
       </property>
       <item>
        <widget name="lblGraphMark" class="QLabel">
         <property name="text">
          <string />
         </property>
        </widget>
       </item>
       <item>
        <widget class="QLabel" name="lblProduct">
         <property name="text">
          <string>TaxiGraph</string>
         </property>
        </widget>
       </item>
       <item>
        <spacer name="spacerAppHeader">
         <property name="sizeHint">
          <size>
           <width>16</width>
           <height>10</height>
          </size>
         </property>
         <property name="orientation">
          <enum>Qt::Horizontal</enum>
         </property>
        </spacer>
       </item>
       <item>
        <widget class="QGroupBox" name="groupSim">
         <property name="sizePolicy">
          <sizepolicy hsizetype="Preferred" vsizetype="Fixed">
           <horstretch>0</horstretch>
           <verstretch>0</verstretch>
          </sizepolicy>
         </property>
         <property name="title">
          <string />
         </property>
         <property name="flat">
          <bool>true</bool>
         </property>
         <layout class="QHBoxLayout" name="gridSim">
          <property name="spacing">
           <number>8</number>
          </property>
          <property name="leftMargin">
           <number>0</number>
          </property>
          <property name="topMargin">
           <number>0</number>
          </property>
          <property name="rightMargin">
           <number>0</number>
          </property>
          <property name="bottomMargin">
           <number>0</number>
          </property>
          <item>
           <widget class="QPushButton" name="btnPause">
            <property name="minimumSize">
             <size>
              <width>102</width>
              <height>34</height>
             </size>
            </property>
            <property name="text">
             <string>Pausar</string>
            </property>
            <property name="checkable">
             <bool>true</bool>
            </property>
           </widget>
          </item>
          <item>
           <widget class="QPushButton" name="btnReset">
            <property name="minimumSize">
             <size>
              <width>94</width>
              <height>34</height>
             </size>
            </property>
            <property name="text">
             <string>Reiniciar</string>
            </property>
           </widget>
          </item>
          <item>
           <widget class="QLabel" name="lblSpeed">
            <property name="text">
             <string>Velocidad</string>
            </property>
            <property name="buddy">
             <cstring>comboSpeed</cstring>
            </property>
           </widget>
          </item>
          <item>
           <widget class="QComboBox" name="comboSpeed">
            <property name="minimumSize">
             <size>
              <width>62</width>
              <height>34</height>
             </size>
            </property>
            <item>
             <property name="text">
              <string>1x</string>
             </property>
            </item>
            <item>
             <property name="text">
              <string>2x</string>
             </property>
            </item>
            <item>
             <property name="text">
              <string>4x</string>
             </property>
            </item>
           </widget>
          </item>
          <item>
           <widget class="QLabel" name="lblTimeMode">
            <property name="text">
             <string>Momento del día</string>
            </property>
            <property name="buddy">
             <cstring>comboTimeMode</cstring>
            </property>
           </widget>
          </item>
          <item>
           <widget class="QComboBox" name="comboTimeMode">
            <property name="minimumSize">
             <size>
              <width>188</width>
              <height>34</height>
             </size>
            </property>
            <item>
             <property name="text">
              <string>Automático (según el reloj)</string>
             </property>
            </item>
            <item>
             <property name="text">
              <string>Normal</string>
             </property>
            </item>
            <item>
             <property name="text">
              <string>Nocturno (+$2,000)</string>
             </property>
            </item>
            <item>
             <property name="text">
              <string>Hora pico (pesos x1.8)</string>
             </property>
            </item>
           </widget>
          </item>
         </layout>
        </widget>
       </item>
      </layout>
     </widget>
    </item>
    <item>
     <widget class="QSplitter" name="splitter">
      <property name="orientation">
       <enum>Qt::Horizontal</enum>
      </property>
      <property name="childrenCollapsible">
       <bool>false</bool>
      </property>
      <property name="handleWidth">
       <number>8</number>
      </property>
      <widget class="QWidget" name="mapHost">
       <property name="sizePolicy">
        <sizepolicy hsizetype="Expanding" vsizetype="Expanding">
         <horstretch>1</horstretch>
         <verstretch>0</verstretch>
        </sizepolicy>
       </property>
       <layout class="QVBoxLayout" name="layoutMapHost">
        <property name="spacing">
         <number>6</number>
        </property>
        <property name="leftMargin">
         <number>0</number>
        </property>
        <property name="topMargin">
         <number>0</number>
        </property>
        <property name="rightMargin">
         <number>0</number>
        </property>
        <property name="bottomMargin">
         <number>0</number>
        </property>
        <item>
         <widget name="graphSummary" class="QWidget">
          <layout name="layoutGraphSummary" class="QHBoxLayout">
           <property name="spacing">
            <number>10</number>
           </property>
           <property name="leftMargin">
            <number>12</number>
           </property>
           <property name="topMargin">
            <number>7</number>
           </property>
           <property name="rightMargin">
            <number>12</number>
           </property>
           <property name="bottomMargin">
            <number>7</number>
           </property>
           <item>
            <widget name="lblGraphTitle" class="QLabel">
             <property name="text">
              <string>Grafo de la ciudad</string>
             </property>
            </widget>
           </item>
           <item>
            <widget name="lblGraphCounts" class="QLabel">
             <property name="text">
              <string />
             </property>
            </widget>
           </item>
           <item>
            <spacer name="spacerGraphSummary">
             <property name="orientation">
              <enum>Qt::Horizontal</enum>
             </property>
             <property name="sizeHint">
              <size>
               <width>8</width>
               <height>8</height>
              </size>
             </property>
            </spacer>
           </item>
           <item>
            <widget name="lblGraphClock" class="QLabel">
             <property name="text">
              <string />
             </property>
            </widget>
           </item>
           <item>
            <widget name="lblGraphPeriod" class="QLabel">
             <property name="text">
              <string />
             </property>
            </widget>
           </item>
          </layout>
         </widget>
        </item>
        <item>
         <widget class="QGroupBox" name="groupEdit">
          <property name="maximumSize">
           <size>
            <width>16777215</width>
            <height>58</height>
           </size>
          </property>
          <property name="sizePolicy">
           <sizepolicy hsizetype="Expanding" vsizetype="Fixed">
            <horstretch>0</horstretch>
            <verstretch>0</verstretch>
           </sizepolicy>
          </property>
          <property name="title">
           <string />
          </property>
          <property name="flat">
           <bool>true</bool>
          </property>
          <layout class="QHBoxLayout" name="layoutEdit" stretch="0,0,1,0">
           <property name="spacing">
            <number>10</number>
           </property>
           <property name="leftMargin">
            <number>12</number>
           </property>
           <property name="topMargin">
            <number>4</number>
           </property>
           <property name="rightMargin">
            <number>12</number>
           </property>
           <property name="bottomMargin">
            <number>4</number>
           </property>
           <item>
            <widget class="QLabel" name="lblMapMode">
             <property name="text">
              <string>Modo</string>
             </property>
             <property name="buddy">
              <cstring>comboEditMode</cstring>
             </property>
            </widget>
           </item>
           <item>
            <widget class="QComboBox" name="comboEditMode">
             <property name="minimumSize">
              <size>
               <width>170</width>
               <height>32</height>
              </size>
             </property>
             <property name="accessibleName">
              <string>Modo del mapa</string>
             </property>
             <item>
              <property name="text">
               <string>Conducir mi taxi</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Agregar nodo</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Agregar calle</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Eliminar calle</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Eliminar nodo</string>
              </property>
             </item>
             <item>
              <property name="text">
               <string>Cambiar peso de una calle</string>
              </property>
             </item>
            </widget>
           </item>
           <item>
            <widget class="QLabel" name="lblEditHint">
             <property name="sizePolicy">
              <sizepolicy hsizetype="Ignored" vsizetype="Preferred">
               <horstretch>0</horstretch>
               <verstretch>0</verstretch>
              </sizepolicy>
             </property>
             <property name="text">
              <string />
             </property>
             <property name="wordWrap">
              <bool>true</bool>
             </property>
            </widget>
           </item>
           <item>
            <widget class="QCheckBox" name="chkWeights">
             <property name="text">
              <string>Mostrar pesos</string>
             </property>
             <property name="checked">
              <bool>false</bool>
             </property>
             <property name="toolTip">
              <string>Muestra el peso de cada calle en el mapa.</string>
             </property>
            </widget>
           </item>
          </layout>
         </widget>
        </item>
        <item>
         <widget name="graphLegend" class="QWidget">
          <layout name="layoutGraphLegend" class="QHBoxLayout">
           <property name="spacing">
            <number>10</number>
           </property>
           <property name="leftMargin">
            <number>12</number>
           </property>
           <property name="topMargin">
            <number>4</number>
           </property>
           <property name="rightMargin">
            <number>12</number>
           </property>
           <property name="bottomMargin">
            <number>4</number>
           </property>
           <item>
            <widget name="lblNodeKey" class="QLabel">
             <property name="text">
              <string>● Nodo</string>
             </property>
            </widget>
           </item>
           <item>
            <widget name="lblOneWayKey" class="QLabel">
             <property name="text">
              <string>› Un sentido</string>
             </property>
            </widget>
           </item>
           <item>
            <widget name="lblTwoWayKey" class="QLabel">
             <property name="text">
              <string>‹ › Doble sentido</string>
             </property>
            </widget>
           </item>
           <item>
            <spacer name="spacerGraphLegend">
             <property name="orientation">
              <enum>Qt::Horizontal</enum>
             </property>
             <property name="sizeHint">
              <size>
               <width>8</width>
               <height>8</height>
              </size>
             </property>
            </spacer>
           </item>
           <item>
            <widget name="lblNodeFocus" class="QLabel">
             <property name="text">
              <string>Pasa el cursor por un nodo</string>
             </property>
             <property name="sizePolicy">
              <sizepolicy hsizetype="Ignored" vsizetype="Preferred">
               <horstretch>1</horstretch>
               <verstretch>0</verstretch>
              </sizepolicy>
             </property>
            </widget>
           </item>
          </layout>
         </widget>
        </item>
       </layout>
      </widget>
      <widget class="QWidget" name="sidePanel">
       <property name="minimumSize">
        <size>
         <width>410</width>
         <height>0</height>
        </size>
       </property>
       <layout class="QVBoxLayout" name="layoutSide" stretch="0,0,1">
        <property name="spacing">
         <number>10</number>
        </property>
        <property name="leftMargin">
         <number>10</number>
        </property>
        <property name="topMargin">
         <number>12</number>
        </property>
        <property name="rightMargin">
         <number>10</number>
        </property>
        <property name="bottomMargin">
         <number>10</number>
        </property>
        <item>
         <widget class="QLabel" name="lblTelemetry">
          <property name="text">
           <string>Flota y recorridos</string>
          </property>
         </widget>
        </item>
        <item>
         <widget name="manualSummary" class="QWidget">
          <layout name="layoutManualSummary" class="QVBoxLayout">
           <property name="spacing">
            <number>3</number>
           </property>
           <property name="leftMargin">
            <number>12</number>
           </property>
           <property name="topMargin">
            <number>7</number>
           </property>
           <property name="rightMargin">
            <number>12</number>
           </property>
           <property name="bottomMargin">
            <number>7</number>
           </property>
           <item>
            <layout name="layoutManualNumbers" class="QHBoxLayout">
             <item>
              <widget name="lblManualTitle" class="QLabel">
               <property name="text">
                <string>Tu taxi</string>
               </property>
              </widget>
             </item>
             <item>
              <spacer name="spacerManualNumbers">
               <property name="orientation">
                <enum>Qt::Horizontal</enum>
               </property>
               <property name="sizeHint">
                <size>
                 <width>8</width>
                 <height>8</height>
                </size>
               </property>
              </spacer>
             </item>
             <item>
              <widget name="lblManualTrips" class="QLabel">
               <property name="text">
                <string />
               </property>
              </widget>
             </item>
             <item>
              <widget name="lblManualIncome" class="QLabel">
               <property name="text">
                <string />
               </property>
              </widget>
             </item>
            </layout>
           </item>
           <item>
            <widget name="lblManualState" class="QLabel">
             <property name="text">
              <string />
             </property>
            </widget>
           </item>
          </layout>
         </widget>
        </item>
        <item>
         <widget class="QTabWidget" name="tabs">
          <property name="sizePolicy">
           <sizepolicy hsizetype="Expanding" vsizetype="Expanding">
            <horstretch>0</horstretch>
            <verstretch>0</verstretch>
           </sizepolicy>
          </property>
          <property name="currentIndex">
           <number>0</number>
          </property>
          <property name="usesScrollButtons">
           <bool>true</bool>
          </property>
          <widget class="QWidget" name="tabEarnings">
           <attribute name="title">
            <string>Taxis</string>
           </attribute>
           <layout class="QVBoxLayout" name="layoutEarnings" stretch="0,0,0,1">
            <property name="spacing">
             <number>8</number>
            </property>
            <property name="leftMargin">
             <number>4</number>
            </property>
            <property name="topMargin">
             <number>8</number>
            </property>
            <property name="rightMargin">
             <number>4</number>
            </property>
            <property name="bottomMargin">
             <number>4</number>
            </property>
            <item>
             <widget name="lblBfs" class="QLabel">
              <property name="text">
               <string>BFS · Ruta con menos saltos</string>
              </property>
             </widget>
            </item>
            <item>
             <widget name="lblDfs" class="QLabel">
              <property name="text">
               <string>DFS · Explora una rama a fondo</string>
              </property>
             </widget>
            </item>
            <item>
             <widget name="lblDijkstra" class="QLabel">
              <property name="text">
               <string>Dijkstra · Ruta de menor peso</string>
              </property>
             </widget>
            </item>
            <item>
             <widget class="QTableWidget" name="tableTaxis">
              <property name="minimumSize">
               <size>
                <width>0</width>
                <height>158</height>
               </size>
              </property>
             </widget>
            </item>
           </layout>
          </widget>
          <widget class="QWidget" name="tabPassengers">
           <attribute name="title">
            <string>Pasajeros</string>
           </attribute>
           <layout class="QVBoxLayout" name="layoutPassengers">
            <property name="spacing">
             <number>8</number>
            </property>
            <property name="leftMargin">
             <number>4</number>
            </property>
            <property name="topMargin">
             <number>8</number>
            </property>
            <property name="rightMargin">
             <number>4</number>
            </property>
            <property name="bottomMargin">
             <number>4</number>
            </property>
            <item>
             <widget class="QTableWidget" name="tablePassengers" />
            </item>
           </layout>
          </widget>
          <widget class="QWidget" name="tabChart">
           <attribute name="title">
            <string>Ingresos</string>
           </attribute>
           <layout class="QVBoxLayout" name="layoutChart" stretch="0,1">
            <property name="spacing">
             <number>8</number>
            </property>
            <property name="leftMargin">
             <number>4</number>
            </property>
            <property name="topMargin">
             <number>8</number>
            </property>
            <property name="rightMargin">
             <number>4</number>
            </property>
            <property name="bottomMargin">
             <number>4</number>
            </property>
            <item>
             <widget class="QWidget" name="barsHost" native="true">
              <layout class="QVBoxLayout" name="layoutBarsHost">
               <property name="leftMargin">
                <number>0</number>
               </property>
               <property name="topMargin">
                <number>0</number>
               </property>
               <property name="rightMargin">
                <number>0</number>
               </property>
               <property name="bottomMargin">
                <number>0</number>
               </property>
              </layout>
             </widget>
            </item>
            <item>
             <widget class="QWidget" name="chartHost" native="true">
              <layout class="QVBoxLayout" name="layoutChartHost">
               <property name="leftMargin">
                <number>0</number>
               </property>
               <property name="topMargin">
                <number>0</number>
               </property>
               <property name="rightMargin">
                <number>0</number>
               </property>
               <property name="bottomMargin">
                <number>0</number>
               </property>
              </layout>
             </widget>
            </item>
           </layout>
          </widget>
          <widget class="QWidget" name="tabLog">
           <attribute name="title">
            <string>Eventos</string>
           </attribute>
           <layout class="QVBoxLayout" name="layoutLog">
            <property name="spacing">
             <number>8</number>
            </property>
            <property name="leftMargin">
             <number>4</number>
            </property>
            <property name="topMargin">
             <number>8</number>
            </property>
            <property name="rightMargin">
             <number>4</number>
            </property>
            <property name="bottomMargin">
             <number>4</number>
            </property>
            <item>
             <widget class="QTextEdit" name="txtLog">
              <property name="readOnly">
               <bool>true</bool>
              </property>
             </widget>
            </item>
           </layout>
          </widget>
          <widget class="QWidget" name="tabInfo">
           <attribute name="title">
            <string>Guía</string>
           </attribute>
           <layout class="QVBoxLayout" name="layoutInfo">
            <property name="spacing">
             <number>8</number>
            </property>
            <property name="leftMargin">
             <number>4</number>
            </property>
            <property name="topMargin">
             <number>8</number>
            </property>
            <property name="rightMargin">
             <number>4</number>
            </property>
            <property name="bottomMargin">
             <number>4</number>
            </property>
            <item>
             <widget class="QTextBrowser" name="txtInfo" />
            </item>
           </layout>
          </widget>
         </widget>
        </item>
       </layout>
      </widget>
     </widget>
    </item>
   </layout>
  </widget>
  <widget class="QStatusBar" name="statusbar" />
 </widget>
 <resources />
 <connections />
</ui>

`

### Archivo: MapaWidget.h

`cpp
#ifndef MAPA_WIDGET_H
#define MAPA_WIDGET_H

#include <QColor>
#include <QFont>
#include <QPointF>
#include <QString>
#include <QTransform>
#include <QVariantAnimation>
#include <QVector>
#include <QWidget>

#include "TextoFlotante.h"

class QPainter;
class Simulacion;
class Taxi;

class MapaWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    QVariantAnimation animacionNodo;
    double oscuridad;
    int nodoBajoMouse;
    int nodoPendiente;
    bool modoConducir;
    bool mostrarPesos;
    QVector<TextoFlotante> textosFlotantes;

    static QFont fuentePixel(int px, bool negrita = false);

    static QColor obtenerColorTipoNodo(QString tipo);

    static QColor mezclarColores(QColor a, QColor b, double t);

    static void dibujarTextoConBorde(QPainter &p, QPointF centro, QString texto,
                                     QFont fuente, QColor relleno, double grosorBorde = 3.5);

    QTransform transformacionVista() const;

    QPointF posicionDibujoTaxi(const Taxi *taxi) const;

    void dibujarMundo(QPainter &p, bool esMinimapa);

    void dibujarEscenario(QPainter &p);

    void dibujarCalles(QPainter &p, bool esMinimapa);

    void dibujarRutas(QPainter &p);

    void dibujarPesos(QPainter &p);

    void dibujarNodos(QPainter &p, bool esMinimapa);

    void dibujarPasajeros(QPainter &p, bool esMinimapa);

    void dibujarTaxis(QPainter &p, bool esMinimapa);

    void dibujarEfectosNocturnos(QPainter &p);


    void dibujarMinimapa(QPainter &p);
    void seleccionarNodoEnFoco(int id);

private slots:
    void enCuadro(double dt);

    void enCarreraCompletada(int, QPointF pos, double tarifa, QColor color);

protected:
    void paintEvent(QPaintEvent *) override;

    void mousePressEvent(QMouseEvent *e) override;

    void mouseMoveEvent(QMouseEvent *e) override;

    void leaveEvent(QEvent *) override;

public:
    explicit MapaWidget(QWidget *parent = nullptr);

    void setSimulacion(Simulacion *nuevaSim);

    void setNodoPendiente(int idNodo);
    void setModoConducir(bool activo);
    void setMostrarPesos(bool mostrar);

signals:
    void nodoClicado(int idNodo);
    void nodoEnFocoCambiado(int idNodo);
    void espacioVacioClicado(QPointF posMapa);
    void cancelarSolicitado();
};

#endif // MAPA_WIDGET_H

`

### Archivo: MapaWidget.cpp

`cpp
#include "MapaWidget.h"

#include "Arista.h"
#include "ElementoArista.h"
#include "ElementoNodo.h"
#include "Grafo.h"
#include "ListaAristas.h"
#include "ListaNodos.h"
#include "ManzanaCiudad.h"
#include "Nodo.h"
#include "Pasajero.h"
#include "RelojSimulacion.h"
#include "Simulacion.h"
#include "Taxi.h"
#include "TaxiManual.h"

#include <QEasingCurve>
#include <QEvent>
#include <QFontMetricsF>
#include <QLineF>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPolygonF>
#include <QRectF>
#include <QtMath>
#include <cmath>

namespace
{
const QColor fondoVista("#101821");
const QColor fondoPanel("#17222e");
const QColor bordePanel("#394a5a");
const QColor textoPrincipal("#e8eef4");
const QColor textoSecundario("#a5b4c3");
const QColor colorManual("#ffd166");
const QColor colorSeleccion("#70d5cc");
const QColor colorTrafico("#fb7185");
}

QFont MapaWidget::fuentePixel(int px, bool negrita)
{
    QFont f;
    f.setFamilies({QStringLiteral("Bahnschrift"), QStringLiteral("Segoe UI")});
    f.setPixelSize(px);
    f.setBold(negrita);
    return f;
}

QColor MapaWidget::obtenerColorTipoNodo(QString tipo)
{
    if (tipo == "Aeropuerto")
    {
        return QColor("#79bdff");
    }
    if (tipo == "Terminal")
    {
        return QColor("#c3b1f6");
    }
    if (tipo == "Turistico")
    {
        return QColor("#f0c77f");
    }
    return QColor("#dce8f2");
}

QColor MapaWidget::mezclarColores(QColor a, QColor b, double t)
{
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    return QColor::fromRgbF(a.redF() + (b.redF() - a.redF()) * t,
                            a.greenF() + (b.greenF() - a.greenF()) * t,
                            a.blueF() + (b.blueF() - a.blueF()) * t);
}

void MapaWidget::dibujarTextoConBorde(QPainter &p, QPointF centro, QString texto,
                                      QFont fuente, QColor relleno, double grosorBorde)
{
    QFontMetricsF fm(fuente);
    QPainterPath rutaTexto;
    rutaTexto.addText(centro.x() - fm.horizontalAdvance(texto) / 2.0, centro.y(), fuente, texto);
    p.setPen(QPen(QColor(8, 12, 20, 235), grosorBorde, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    p.drawPath(rutaTexto);
    p.setPen(Qt::NoPen);
    p.setBrush(relleno);
    p.drawPath(rutaTexto);
}

QTransform MapaWidget::transformacionVista() const
{
    QRectF b = sim->getLimitesMapa();
    double margen = 8.0;
    double escala = qMin((width() - 2 * margen) / b.width(),
                         (height() - 2 * margen) / b.height());
    QTransform t;
    t.translate((width() - b.width() * escala) / 2.0, (height() - b.height() * escala) / 2.0);
    t.scale(escala, escala);
    return t;
}

QPointF MapaWidget::posicionDibujoTaxi(const Taxi *taxi) const
{
    double rad = qDegreesToRadians(taxi->getAngulo());
    return taxi->getPosicion() + QPointF(-std::sin(rad), std::cos(rad)) * 6.0;
}

void MapaWidget::dibujarMundo(QPainter &p, bool esMinimapa)
{
    dibujarEscenario(p);
    dibujarCalles(p, esMinimapa);
    if (!esMinimapa)
    {
        dibujarRutas(p);
        if (mostrarPesos)
        {
            dibujarPesos(p);
        }
    }
    dibujarNodos(p, esMinimapa);
    dibujarPasajeros(p, esMinimapa);
    dibujarTaxis(p, esMinimapa);
    dibujarEfectosNocturnos(p);

    if (!esMinimapa)
    {
        for (int i = 0; i < textosFlotantes.size(); i++)
        {
            const TextoFlotante &tf = textosFlotantes[i];
            double k = tf.getEdad() / 2.2;
            QColor c = tf.getColor().lighter(120);
            const double avance = QEasingCurve(QEasingCurve::OutCubic).valueForProgress(k);
            const double entrada = qBound(0.0, tf.getEdad() / 0.12, 1.0);
            const double salida = qBound(0.0, (1.0 - k) / 0.42, 1.0);
            c.setAlphaF(entrada * salida);
            dibujarTextoConBorde(p, tf.getPosicion() + QPointF(0, -40 - 36 * avance),
                                 tf.getTexto(), fuentePixel(24, true), c, 3.5);
        }
    }
}

void MapaWidget::dibujarEscenario(QPainter &p)
{
    QRectF b = sim->getLimitesMapa();
    QLinearGradient fondo(b.topLeft(), b.bottomRight());
    fondo.setColorAt(0.0, QColor("#1c2b3b"));
    fondo.setColorAt(1.0, QColor("#142330"));
    p.fillRect(b, fondo);
    p.setPen(QPen(QColor(162, 184, 202, 14), 1));
    for (double x = b.left() + 48.0; x < b.right(); x += 48.0)
    {
        p.drawLine(QPointF(x, b.top()), QPointF(x, b.bottom()));
    }
    for (double y = b.top() + 48.0; y < b.bottom(); y += 48.0)
    {
        p.drawLine(QPointF(b.left(), y), QPointF(b.right(), y));
    }

    p.setPen(QPen(QColor("#2e5970"), 3));
    p.setBrush(QColor("#113246"));
    p.drawPolygon(sim->getPoligonoAgua());

    const QVector<ManzanaCiudad> &manzanas = sim->getManzanas();
    for (int i = 0; i < manzanas.size(); i++)
    {
        const ManzanaCiudad &m = manzanas[i];
        QColor color("#243546");
        if (m.getTipo() == "Parque")
        {
            color = QColor("#234637");
        }
        else if (m.getTipo() == "Industrial")
        {
            color = QColor("#424039");
        }
        else if (m.getTipo() == "Aeropuerto")
        {
            color = QColor("#293b50");
        }
        else if (m.getTipo() == "Terminal")
        {
            color = QColor("#39364c");
        }

        p.setPen(QPen(color.lighter(125), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(color);
        p.drawPolygon(m.getPoligono());
    }
}

void MapaWidget::dibujarCalles(QPainter &p, bool esMinimapa)
{
    Grafo *g = sim->getGrafo();

    ElementoArista *elem = g->getAristas()->getPrimero();
    while (elem != nullptr)
    {
        Arista *arista = elem->getArista();
        elem = elem->getSiguiente();
        if (arista == nullptr)
        {
            continue;
        }

        bool tieneInversa = g->existeAristaDirigida(arista->getDestino(), arista->getOrigen());
        if (tieneInversa && arista->getOrigen() > arista->getDestino())
        {
            continue;
        }

        Nodo *na = g->buscarNodo(arista->getOrigen());
        Nodo *nb = g->buscarNodo(arista->getDestino());
        if (na == nullptr || nb == nullptr)
        {
            continue;
        }

        bool av = arista->getEsAvenida();
        p.setPen(QPen(QColor("#101821"), av ? 22 : 16, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(na->getPosicion(), nb->getPosicion());
    }

    elem = g->getAristas()->getPrimero();
    while (elem != nullptr)
    {
        Arista *arista = elem->getArista();
        elem = elem->getSiguiente();
        if (arista == nullptr)
        {
            continue;
        }

        bool tieneInversa = g->existeAristaDirigida(arista->getDestino(), arista->getOrigen());
        if (tieneInversa && arista->getOrigen() > arista->getDestino())
        {
            continue;
        }

        Nodo *na = g->buscarNodo(arista->getOrigen());
        Nodo *nb = g->buscarNodo(arista->getDestino());
        if (na == nullptr || nb == nullptr)
        {
            continue;
        }

        bool av = arista->getEsAvenida();
        QColor base = av ? QColor("#a2b8ca") : QColor("#73899e");
        QColor c = mezclarColores(base, colorTrafico,
                                  qBound(0.0, (g->obtenerNivelCongestion(arista) - 1.05) * 0.30, 0.58));
        p.setPen(QPen(c, av ? 16 : 10, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(na->getPosicion(), nb->getPosicion());
    }

    if (esMinimapa)
    {
        return;
    }

    elem = g->getAristas()->getPrimero();
    while (elem != nullptr)
    {
        Arista *arista = elem->getArista();
        elem = elem->getSiguiente();
        if (arista == nullptr)
        {
            continue;
        }

        bool tieneInversa = g->existeAristaDirigida(arista->getDestino(), arista->getOrigen());
        if (tieneInversa && arista->getOrigen() > arista->getDestino())
        {
            continue;
        }

        Nodo *na = g->buscarNodo(arista->getOrigen());
        Nodo *nb = g->buscarNodo(arista->getDestino());
        if (na == nullptr || nb == nullptr)
        {
            continue;
        }

        QPointF a = na->getPosicion();
        QPointF b = nb->getPosicion();
        const QLineF linea(a, b);
        if (linea.length() < 34.0)
        {
            continue;
        }

        const QPointF sentido = (b - a) / linea.length();
        p.setPen(QPen(QColor("#142430"), 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto chevron = [&p](QPointF centro, QPointF direccion) {
            const QPointF lado(-direccion.y(), direccion.x());
            const QPointF puntos[] = {
                centro - direccion * 4.5 + lado * 3.2,
                centro + direccion * 4.5,
                centro - direccion * 4.5 - lado * 3.2
            };
            p.drawPolyline(puntos, 3);
        };
        chevron(linea.pointAt(0.64), sentido);
        if (tieneInversa)
        {
            chevron(linea.pointAt(0.36), -sentido);
        }
    }
}

void MapaWidget::dibujarRutas(QPainter &p)
{
    Grafo *g = sim->getGrafo();
    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        QPolygonF poli;
        poli << taxi->getPosicion();

        if (taxi->getEnMovimiento() && g->buscarNodo(taxi->getNodoSiguiente()) != nullptr)
        {
            poli << g->buscarNodo(taxi->getNodoSiguiente())->getPosicion();
        }

        for (int k = 0; k < taxi->getCantidadRuta(); k++)
        {
            Nodo *n = g->buscarNodo(taxi->getPasoRuta(k));
            if (n != nullptr)
            {
                poli << n->getPosicion();
            }
        }

        if (poli.size() < 2)
        {
            continue;
        }

        QColor c = taxi->esManual() ? colorManual : taxi->getColor();
        c.setAlpha(taxi->esManual() ? 255 : 180);
        p.setPen(QPen(c, taxi->esManual() ? 6 : 3.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        p.drawPolyline(poli);
    }
}

void MapaWidget::dibujarPesos(QPainter &p)
{
    Grafo *g = sim->getGrafo();
    QFont fuente = fuentePixel(12, true);
    QFontMetricsF fm(fuente);
    p.setFont(fuente);

    ElementoArista *elem = g->getAristas()->getPrimero();
    while (elem != nullptr)
    {
        Arista *a = elem->getArista();
        elem = elem->getSiguiente();
        if (a == nullptr)
        {
            continue;
        }

        if (a->getOrigen() > a->getDestino()
            && g->existeAristaDirigida(a->getDestino(), a->getOrigen()))
        {
            continue;
        }

        Nodo *n1 = g->buscarNodo(a->getOrigen());
        Nodo *n2 = g->buscarNodo(a->getDestino());
        if (n1 == nullptr || n2 == nullptr)
        {
            continue;
        }

        QPointF centro = (n1->getPosicion() + n2->getPosicion()) / 2.0;
        QString texto = QString::number(g->obtenerPesoEfectivoArista(a), 'f', 1);
        QColor tono = mezclarColores(textoPrincipal, colorTrafico,
                                     (g->obtenerNivelCongestion(a) - 1.05) / 1.6);
        double ancho = fm.horizontalAdvance(texto) + 10;
        QRectF caja(centro.x() - ancho / 2, centro.y() - 8, ancho, 16);
        p.setPen(QPen(bordePanel, 1.5));
        p.setBrush(fondoPanel);
        p.drawRoundedRect(caja, 4, 4);
        p.setPen(tono);
        p.drawText(caja, Qt::AlignCenter, texto);
    }
}

void MapaWidget::dibujarNodos(QPainter &p, bool esMinimapa)
{
    Grafo *g = sim->getGrafo();

    ElementoNodo *elem = g->getNodos()->getPrimero();
    while (elem != nullptr)
    {
        Nodo *n = elem->getNodo();
        elem = elem->getSiguiente();
        if (n == nullptr)
        {
            continue;
        }

        QColor colorTipo = obtenerColorTipoNodo(n->getTipo());
        bool especial = (n->getTipo() != "Normal");
        double radio = especial ? (esMinimapa ? 13 : 16) : 8.5;

        if (!esMinimapa && n->getId() == nodoBajoMouse)
        {
            const double avance = animacionNodo.currentValue().toDouble();
            QColor halo = colorSeleccion;
            halo.setAlphaF(0.68 * avance);
            p.setPen(QPen(halo, 2.5));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(n->getPosicion(), radio + 7.0 * avance, radio + 7.0 * avance);
        }

        p.setPen(QPen(fondoVista, especial ? 3.2 : 2.6));
        p.setBrush(colorTipo);
        p.drawEllipse(n->getPosicion(), radio, radio);

        if (especial)
        {
            p.save();
            p.translate(n->getPosicion());
            p.setPen(QPen(fondoVista, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(fondoVista);

            if (n->getTipo() == "Aeropuerto")
            {
                p.drawLine(QPointF(0, -8), QPointF(0, 8));
                const QPointF alas[] = {QPointF(-8, 2), QPointF(0, -2), QPointF(8, 2)};
                const QPointF cola[] = {QPointF(-4, 7), QPointF(0, 5), QPointF(4, 7)};
                p.drawPolyline(alas, 3);
                p.drawPolyline(cola, 3);
            }
            else if (n->getTipo() == "Terminal")
            {
                p.drawRoundedRect(QRectF(-7, -7, 14, 13), 3, 3);
                p.setPen(QPen(colorTipo, 2));
                p.drawLine(QPointF(-3, -2), QPointF(3, -2));
                p.drawLine(QPointF(-3, 2), QPointF(3, 2));
                p.setPen(QPen(fondoVista, 2.2, Qt::SolidLine, Qt::RoundCap));
                p.drawLine(QPointF(-4, 6), QPointF(-4, 8));
                p.drawLine(QPointF(4, 6), QPointF(4, 8));
            }
            else
            {
                p.setPen(Qt::NoPen);
                QPolygonF estrella;
                for (int i = 0; i < 10; ++i)
                {
                    const double angulo = qDegreesToRadians(i * 36.0 - 90.0);
                    const double r = i % 2 == 0 ? 9.0 : 4.4;
                    estrella << QPointF(r * std::cos(angulo), r * std::sin(angulo));
                }
                p.drawPolygon(estrella);
            }
            p.restore();
        }

        if (!esMinimapa)
        {
            bool resaltado = (n->getId() == nodoBajoMouse);
            const double escala = p.worldTransform().m11();
            const int px = qBound(especial ? 16 : 14, qRound(9.0 / escala), 24);
            const QFont f = fuentePixel(px, especial || resaltado);
            const QColor c = resaltado ? QColor(Qt::white) : (especial ? colorTipo : textoPrincipal);
            const QString nombre = resaltado ? n->getNombre()
                                            : QFontMetricsF(f).elidedText(n->getNombre(), Qt::ElideRight, 220);
            dibujarTextoConBorde(p, n->getPosicion() + QPointF(0, especial ? 35 : 27),
                                 nombre, f, c);
        }
    }

    if (esMinimapa)
    {
        return;
    }

    if (g->buscarNodo(nodoPendiente) != nullptr)
    {
        p.setPen(QPen(colorSeleccion, 3.5));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(g->buscarNodo(nodoPendiente)->getPosicion(), 25, 25);
    }

    if (modoConducir && sim->getTaxiManual() != nullptr)
    {
        constexpr double r = 23.0;
        p.setPen(QPen(colorManual, 2.6, Qt::DashLine));
        QColor relleno = colorManual;
        relleno.setAlpha(36);
        p.setBrush(relleno);

        int desde = sim->getTaxiManual()->nodoFinalDeRuta();
        ElementoArista *ea = g->getAristas()->getPrimero();
        while (ea != nullptr)
        {
            Arista *a = ea->getArista();
            if (a != nullptr && a->getOrigen() == desde)
            {
                Nodo *vecino = g->buscarNodo(a->getDestino());
                if (vecino != nullptr)
                {
                    p.drawEllipse(vecino->getPosicion(), r, r);
                }
            }
            ea = ea->getSiguiente();
        }
    }
}

void MapaWidget::dibujarPasajeros(QPainter &p, bool esMinimapa)
{
    Grafo *g = sim->getGrafo();
    const QVector<Pasajero> &lista = sim->getPasajerosEnEspera();

    for (int i = 0; i < lista.size(); i++)
    {
        const Pasajero &pas = lista[i];
        Nodo *no = g->buscarNodo(pas.getOrigen());
        Nodo *nd = g->buscarNodo(pas.getDestino());
        if (no == nullptr || nd == nullptr)
        {
            continue;
        }

        QPointF o = no->getPosicion();
        bool libre = !pas.estaReservado();
        QColor colorAro = colorSeleccion;

        if (!libre)
        {
            for (int k = 0; k < sim->getCantidadTaxis(); k++)
            {
                Taxi *tx = sim->getTaxi(k);
                if (tx->getId() == pas.getIdTaxiAsignado())
                {
                    colorAro = tx->esManual() ? colorManual : tx->getColor();
                }
            }
        }

        double r = libre ? 21.0 : 17.0;
        p.setPen(QPen(colorAro, libre ? 2.6 : 1.8));
        p.setBrush(QColor(colorAro.red(), colorAro.green(), colorAro.blue(), libre ? 32 : 20));
        p.drawEllipse(o, r, r);

        p.save();
        p.translate(o + QPointF(0, -26));
        p.setPen(QPen(QColor(8, 12, 20), 2));
        p.setBrush(colorAro);
        p.drawEllipse(QPointF(0, -6), 5.0, 5.0);
        p.drawRoundedRect(QRectF(-6, 0, 12, 10), 4, 4);
        p.restore();

        if (libre && !esMinimapa && no->getId() == nodoBajoMouse)
        {
            QString texto = QStringLiteral("Pasajero → %1  ·  %2")
                                .arg(nd->getNombre(),
                                     Grafo::formatearPesos(sim->cotizarCarrera(pas.getOrigen(), pas.getDestino())));
            QFont f = fuentePixel(12, true);
            QFontMetricsF fm(f);
            double w = fm.horizontalAdvance(texto) + 14;
            QRectF globo(o.x() - w / 2, o.y() - 61, w, 20);
            p.setPen(QPen(colorAro, 1.8));
            p.setBrush(fondoPanel);
            p.drawRoundedRect(globo, 7, 7);
            p.setFont(f);
            p.setPen(QColor(255, 255, 255));
            p.drawText(globo, Qt::AlignCenter, texto);
        }
    }

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        if (!taxi->getLlevaPasajero())
        {
            continue;
        }
        Nodo *nd = g->buscarNodo(taxi->getDestinoPasajeroABordo());
        if (nd == nullptr)
        {
            continue;
        }
        QPointF d = nd->getPosicion();
        p.save();
        p.translate(d + QPointF(0, -34));
        QPolygonF rombo;
        rombo << QPointF(0, -11) << QPointF(9, 0) << QPointF(0, 11) << QPointF(-9, 0);
        p.setPen(QPen(QColor(8, 12, 20), 2.5));
        p.setBrush(taxi->esManual() ? colorManual : taxi->getColor());
        p.drawPolygon(rombo);
        p.restore();

        if (taxi->esManual() && !esMinimapa)
        {
            dibujarTextoConBorde(p, d + QPointF(0, -54), QStringLiteral("DESTINO"),
                                 fuentePixel(14, true), colorManual);
        }
    }
}

void MapaWidget::dibujarTaxis(QPainter &p, bool esMinimapa)
{
    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        QPointF pos = posicionDibujoTaxi(taxi);

        p.save();
        p.translate(pos);
        p.rotate(taxi->getAngulo());
        const QColor color = taxi->esManual() ? colorManual : taxi->getColor();
        if (!esMinimapa)
        {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(3, 8, 14, 125));
            p.drawRoundedRect(QRectF(-16, -6, 35, 19), 5, 5);
        }
        p.setPen(QPen(fondoVista, 2.4));
        p.setBrush(color);
        p.drawRoundedRect(QRectF(-16, -9, 32, 18), 5, 5);
        p.setPen(Qt::NoPen);
        p.setBrush(fondoVista);
        p.drawRoundedRect(QRectF(3, -6, 6, 12), 2, 2);
        p.drawRoundedRect(QRectF(-11, -6, 4, 12), 2, 2);
        p.setBrush(color.lighter(150));
        p.drawRoundedRect(QRectF(12, -6.5, 3, 4), 1, 1);
        p.drawRoundedRect(QRectF(12, 2.5, 3, 4), 1, 1);
        p.restore();

        if (esMinimapa)
        {
            continue;
        }

        QString etiqueta = taxi->getNombre();
        if (taxi->getLlevaPasajero())
        {
            etiqueta += QStringLiteral(" ●");
        }
        QFont f = fuentePixel(12, true);
        QFontMetricsF fm(f);
        double w = fm.horizontalAdvance(etiqueta) + 12;
        QRectF caja(pos.x() - w / 2, pos.y() - 36, w, 17);
        p.setPen(QPen(taxi->esManual() ? colorManual : bordePanel, 1.6));
        p.setBrush(taxi->esManual() ? colorManual : fondoPanel);
        p.drawRoundedRect(caja, 5, 5);
        p.setFont(f);
        p.setPen(taxi->esManual() ? fondoVista : textoPrincipal);
        p.drawText(caja, Qt::AlignCenter, etiqueta);
    }
}

void MapaWidget::dibujarEfectosNocturnos(QPainter &p)
{
    if (oscuridad < 0.01)
    {
        return;
    }

    p.fillRect(sim->getLimitesMapa(), QColor(6, 12, 46, static_cast<int>(96 * oscuridad)));

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        p.save();
        p.translate(posicionDibujoTaxi(taxi));
        p.rotate(taxi->getAngulo());
        QLinearGradient haz(10, 0, 70, 0);
        haz.setColorAt(0.0, QColor(255, 244, 200, static_cast<int>(170 * oscuridad)));
        haz.setColorAt(1.0, QColor(255, 244, 200, 0));
        p.setPen(Qt::NoPen);
        p.setBrush(haz);
        QPolygonF cono;
        cono << QPointF(10, -5) << QPointF(70, -28) << QPointF(70, 28) << QPointF(10, 5);
        p.drawPolygon(cono);
        p.restore();
    }
}

void MapaWidget::dibujarMinimapa(QPainter &p)
{
    TaxiManual *yo = sim->getTaxiManual();
    if (yo == nullptr || width() < 1000 || height() < 575)
    {
        return;
    }

    const QRectF panel(14, height() - 110, 138, 96);
    const QRectF vista = panel.adjusted(1, 19, -1, -1);

    p.setPen(QPen(bordePanel, 1));
    p.setBrush(fondoPanel);
    p.drawRoundedRect(panel, 8, 8);

    QPainterPath recorte;
    recorte.addRoundedRect(vista, 6, 6);
    p.save();
    p.setClipPath(recorte);
    p.fillRect(vista, fondoVista);

    QTransform t;
    t.translate(vista.center().x(), vista.center().y());
    t.scale(0.20, 0.20);
    t.translate(-yo->getPosicion().x(), -yo->getPosicion().y());
    p.setTransform(t);
    dibujarMundo(p, true);
    p.restore();

    p.setFont(fuentePixel(10, true));
    p.setPen(textoSecundario);
    p.drawText(panel.adjusted(10, 1, -10, -77), Qt::AlignVCenter | Qt::AlignLeft,
               QStringLiteral("Minimapa"));
}

void MapaWidget::enCuadro(double dt)
{
    double meta = sim->getReloj().oscuridad();
    double factor = dt * 2.5;
    if (factor > 1.0) factor = 1.0;
    oscuridad += (meta - oscuridad) * factor;

    for (int i = textosFlotantes.size() - 1; i >= 0; i--)
    {
        textosFlotantes[i].avanzarEdad(dt);
        if (textosFlotantes[i].getEdad() > 2.2)
        {
            textosFlotantes.removeAt(i);
        }
    }
    update();
}

void MapaWidget::enCarreraCompletada(int, QPointF pos, double tarifa, QColor color)
{
    textosFlotantes.append(
        TextoFlotante(pos, QStringLiteral("+") + Grafo::formatearPesos(tarifa), color));
}

void MapaWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.fillRect(rect(), QColor("#0d141b"));
    const QRectF borde = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    QPainterPath recorte;
    recorte.addRoundedRect(borde, 4, 4);
    p.setClipPath(recorte);
    p.fillRect(borde.adjusted(1, 1, -1, -1), fondoVista);
    if (sim == nullptr)
    {
        return;
    }

    p.save();
    p.setTransform(transformacionVista());
    p.setClipRect(sim->getLimitesMapa(), Qt::IntersectClip);
    dibujarMundo(p, false);
    p.restore();

    dibujarMinimapa(p);
    p.setClipping(false);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(bordePanel, 1));
    p.drawRoundedRect(borde, 4, 4);
}

void MapaWidget::mousePressEvent(QMouseEvent *e)
{
    if (sim == nullptr)
    {
        return;
    }
    if (e->button() == Qt::RightButton)
    {
        emit cancelarSolicitado();
        return;
    }
    if (e->button() != Qt::LeftButton)
    {
        return;
    }

    QPointF posMapa = transformacionVista().inverted().map(QPointF(e->pos()));
    int idNodo = sim->getGrafo()->nodoMasCercano(posMapa, 30.0);
    if (idNodo >= 0)
    {
        emit nodoClicado(idNodo);
    }
    else if (sim->getLimitesMapa().contains(posMapa))
    {
        emit espacioVacioClicado(posMapa);
    }
}

void MapaWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (sim == nullptr)
    {
        return;
    }
    QPointF posMapa = transformacionVista().inverted().map(QPointF(e->pos()));
    seleccionarNodoEnFoco(sim->getGrafo()->nodoMasCercano(posMapa, 30.0));
}

void MapaWidget::leaveEvent(QEvent *)
{
    seleccionarNodoEnFoco(-1);
}

void MapaWidget::seleccionarNodoEnFoco(int id)
{
    if (nodoBajoMouse == id)
    {
        return;
    }

    nodoBajoMouse = id;
    setCursor(id >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    animacionNodo.stop();
    if (id >= 0)
    {
        animacionNodo.start();
    }
    emit nodoEnFocoCambiado(id);
    update();
}

MapaWidget::MapaWidget(QWidget *parent) : QWidget(parent)
{
    sim = nullptr;
    oscuridad = 0.0;
    nodoBajoMouse = -1;
    nodoPendiente = -1;
    modoConducir = true;
    mostrarPesos = true;

    setMouseTracking(true);
    setMinimumSize(560, 420);
    animacionNodo.setDuration(145);
    animacionNodo.setStartValue(0.0);
    animacionNodo.setEndValue(1.0);
    animacionNodo.setEasingCurve(QEasingCurve::OutCubic);
    connect(&animacionNodo, &QVariantAnimation::valueChanged, this, [this] { update(); });
}

void MapaWidget::setSimulacion(Simulacion *nuevaSim)
{
    sim = nuevaSim;
    connect(sim, &Simulacion::cuadroActualizado, this, &MapaWidget::enCuadro);
    connect(sim, &Simulacion::carreraCompletada, this, &MapaWidget::enCarreraCompletada);
    connect(sim->getGrafo(), &Grafo::grafoModificado, this, [this] {
        if (!sim->getGrafo()->tieneNodo(nodoBajoMouse))
        {
            seleccionarNodoEnFoco(-1);
        }
        update();
    });
}

void MapaWidget::setNodoPendiente(int idNodo)
{
    nodoPendiente = idNodo;
}

void MapaWidget::setModoConducir(bool activo)
{
    modoConducir = activo;
}

void MapaWidget::setMostrarPesos(bool mostrar)
{
    mostrarPesos = mostrar;
    update();
}

`

### Archivo: GraficoGananciasWidget.h

`cpp
#ifndef GRAFICO_GANANCIAS_WIDGET_H
#define GRAFICO_GANANCIAS_WIDGET_H

#include <QWidget>

class QChart;
class QChartView;
class QLineSeries;
class QValueAxis;
class Simulacion;

class GraficoGananciasWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    QChart *grafico;
    QChartView *vista;
    QValueAxis *ejeX;
    QValueAxis *ejeY;
    QLineSeries *series[7];
    int cantidadSeries;

private slots:
    void reconstruirSeries();

    void enMuestra(double segundos);

public:
    explicit GraficoGananciasWidget(QWidget *parent = nullptr);

    void setSimulacion(Simulacion *nuevaSim);
};

#endif // GRAFICO_GANANCIAS_WIDGET_H

`

### Archivo: GraficoGananciasWidget.cpp

`cpp
#include "GraficoGananciasWidget.h"

#include "Simulacion.h"
#include "Taxi.h"

#include <QChart>
#include <QChartView>
#include <QFrame>
#include <QColor>
#include <QLegend>
#include <QLineSeries>
#include <QMargins>
#include <QPainter>
#include <QPen>
#include <QString>
#include <QVBoxLayout>
#include <QValueAxis>

void GraficoGananciasWidget::reconstruirSeries()
{
    for (int i = 0; i < cantidadSeries; i++)
    {
        grafico->removeSeries(series[i]);
        delete series[i];
        series[i] = nullptr;
    }
    cantidadSeries = 0;

    if (sim == nullptr)
    {
        return;
    }

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        QLineSeries *s = new QLineSeries();
        s->setName(taxi->getNombre());
        QPen pluma(taxi->getColor());
        pluma.setWidthF(taxi->esManual() ? 3.5 : 2.2);
        s->setPen(pluma);
        grafico->addSeries(s);
        s->attachAxis(ejeX);
        s->attachAxis(ejeY);
        series[cantidadSeries] = s;
        cantidadSeries++;
    }

    ejeX->setRange(0, 90);
    ejeY->setRange(0, 50000);
}

void GraficoGananciasWidget::enMuestra(double segundos)
{
    if (sim == nullptr)
    {
        return;
    }
    if (cantidadSeries != sim->getCantidadTaxis())
    {
        reconstruirSeries();
    }

    double maxIngresos = 50000.0;
    for (int i = 0; i < cantidadSeries; i++)
    {
        Taxi *tx = sim->getTaxi(i);
        series[i]->append(segundos, tx->getIngresos());
        while (series[i]->count() > 1 && series[i]->at(0).x() < segundos - 90.0)
        {
            series[i]->remove(0);
        }
        if (tx->getIngresos() * 1.15 > maxIngresos)
        {
            maxIngresos = tx->getIngresos() * 1.15;
        }
    }

    double x0 = segundos - 90.0;
    if (x0 < 0.0) x0 = 0.0;
    ejeX->setRange(x0, x0 + 90.0);
    ejeY->setRange(0, maxIngresos);
}

GraficoGananciasWidget::GraficoGananciasWidget(QWidget *parent) : QWidget(parent)
{
    sim = nullptr;
    cantidadSeries = 0;
    for (int i = 0; i < 7; i++)
    {
        series[i] = nullptr;
    }

    grafico = new QChart();
    grafico->setTheme(QChart::ChartThemeDark);
    grafico->setBackgroundBrush(QColor("#131d28"));
    grafico->setBackgroundRoundness(9);
    grafico->setPlotAreaBackgroundVisible(true);
    grafico->setPlotAreaBackgroundBrush(QColor("#17222e"));
    grafico->setTitle(QStringLiteral("Ingresos acumulados por taxi"));
    grafico->setTitleBrush(QColor("#e8eef4"));
    grafico->legend()->setLabelColor(QColor("#b8c8d8"));
    grafico->legend()->setAlignment(Qt::AlignRight);
    grafico->setMargins(QMargins(4, 4, 4, 4));

    ejeX = new QValueAxis();
    ejeX->setTitleText(QStringLiteral("Tiempo simulado (s)"));
    ejeX->setTickCount(5);
    ejeX->setLabelFormat("%.0f");
    ejeX->setRange(0, 90);

    ejeY = new QValueAxis();
    ejeY->setTickCount(5);
    ejeY->setLabelFormat("%.0f");
    ejeY->setRange(0, 50000);

    grafico->addAxis(ejeX, Qt::AlignBottom);
    grafico->addAxis(ejeY, Qt::AlignLeft);
    for (QValueAxis *eje : {ejeX, ejeY})
    {
        eje->setLabelsColor(QColor("#a5b4c3"));
        eje->setGridLineColor(QColor("#2b3a49"));
        eje->setLinePenColor(QColor("#526579"));
        eje->setTitleBrush(QColor("#c5d2de"));
    }

    vista = new QChartView(grafico, this);
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setBackgroundBrush(QColor("#17222e"));
    vista->setFrameShape(QFrame::NoFrame);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(vista);
}

void GraficoGananciasWidget::setSimulacion(Simulacion *nuevaSim)
{
    sim = nuevaSim;
    connect(sim, &Simulacion::muestraGrafico, this, &GraficoGananciasWidget::enMuestra);
    connect(sim, &Simulacion::reinicioCompletado, this, &GraficoGananciasWidget::reconstruirSeries);
    reconstruirSeries();
}

`

### Archivo: BarrasGananciasWidget.h

`cpp
#ifndef BARRAS_GANANCIAS_WIDGET_H
#define BARRAS_GANANCIAS_WIDGET_H

#include <QWidget>

class Simulacion;

class BarrasGananciasWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    double mostrado[7];
    double escala;

private slots:
    void reiniciarBarras();

    void enCuadro(double dt);

protected:
    void paintEvent(QPaintEvent *) override;

public:
    explicit BarrasGananciasWidget(QWidget *parent = nullptr);

    void setSimulacion(Simulacion *nuevaSim);
};

#endif // BARRAS_GANANCIAS_WIDGET_H

`

### Archivo: BarrasGananciasWidget.cpp

`cpp
#include "BarrasGananciasWidget.h"

#include "Grafo.h"
#include "Simulacion.h"
#include "Taxi.h"

#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QRectF>

void BarrasGananciasWidget::reiniciarBarras()
{
    for (int i = 0; i < 7; i++)
    {
        mostrado[i] = 0.0;
    }
    escala = 50000.0;
}

void BarrasGananciasWidget::enCuadro(double dt)
{
    if (sim == nullptr)
    {
        return;
    }

    double maximo = 50000.0;
    double factor = dt * 5.0;
    if (factor > 1.0) factor = 1.0;

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *tx = sim->getTaxi(i);
        mostrado[i] += (tx->getIngresos() - mostrado[i]) * factor;
        if (mostrado[i] * 1.15 > maximo)
        {
            maximo = mostrado[i] * 1.15;
        }
    }

    double factorEscala = dt * 3.0;
    if (factorEscala > 1.0) factorEscala = 1.0;
    escala += (maximo - escala) * factorEscala;
    update();
}

void BarrasGananciasWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#2b3a49"), 1));
    p.setBrush(QColor("#131d28"));
    p.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 8, 8);

    if (sim == nullptr || sim->getCantidadTaxis() == 0)
    {
        return;
    }

    int n = sim->getCantidadTaxis();
    double margen = 10.0;
    double anchoEtiqueta = 86.0;
    double anchoValor = 98.0;
    double altoFila = (height() - 2 * margen) / n;
    double altoBarra = qMin(altoFila - 8.0, 12.0);
    double xPista = margen + anchoEtiqueta;
    double anchoPista = width() - 2 * margen - anchoEtiqueta - anchoValor;
    if (anchoPista < 40.0) anchoPista = 40.0;

    int lider = 0;
    for (int i = 1; i < n; i++)
    {
        if (mostrado[i] > mostrado[lider])
        {
            lider = i;
        }
    }

    QFont fuente = p.font();
    fuente.setPixelSize(13);
    fuente.setWeight(QFont::DemiBold);
    p.setFont(fuente);

    for (int i = 0; i < n; i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        double cy = margen + altoFila * (i + 0.5);
        QColor c = taxi->getColor();

        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawEllipse(QPointF(margen + 6, cy), 4.0, 4.0);
        p.setPen(QColor("#e8eef4"));
        p.drawText(QRectF(margen + 18, cy - 11, anchoEtiqueta - 18, 22),
                   Qt::AlignVCenter | Qt::AlignLeft, taxi->getNombre());

        QRectF pista(xPista, cy - altoBarra / 2, anchoPista, altoBarra);
        p.setPen(i == lider && mostrado[i] > 0 ? QPen(QColor("#ffd166"), 1.0) : Qt::NoPen);
        p.setBrush(QColor("#263645"));
        p.drawRoundedRect(pista, 5, 5);

        double fraccion = mostrado[i] / escala;
        if (fraccion < 0.0) fraccion = 0.0;
        if (fraccion > 1.0) fraccion = 1.0;

        if (fraccion > 0.002)
        {
            double anchoRelleno = anchoPista * fraccion;
            if (anchoRelleno < altoBarra) anchoRelleno = altoBarra;
            QRectF relleno(xPista, cy - altoBarra / 2, anchoRelleno, altoBarra);
            p.setPen(Qt::NoPen);
            p.setBrush(c);
            p.drawRoundedRect(relleno, 5, 5);
        }

        p.setPen(i == lider && mostrado[i] > 0 ? QColor("#ffd166") : QColor("#d7e2ec"));
        p.drawText(QRectF(width() - margen - anchoValor, cy - 11, anchoValor, 22),
                   Qt::AlignVCenter | Qt::AlignRight, Grafo::formatearPesos(mostrado[i]));
    }
}

BarrasGananciasWidget::BarrasGananciasWidget(QWidget *parent) : QWidget(parent)
{
    sim = nullptr;
    for (int i = 0; i < 7; i++)
    {
        mostrado[i] = 0.0;
    }
    escala = 50000.0;
    setMinimumHeight(190);
    setMaximumHeight(248);
}

void BarrasGananciasWidget::setSimulacion(Simulacion *nuevaSim)
{
    sim = nuevaSim;
    connect(sim, &Simulacion::cuadroActualizado, this, &BarrasGananciasWidget::enCuadro);
    connect(sim, &Simulacion::reinicioCompletado, this, &BarrasGananciasWidget::reiniciarBarras);
}

`

### Archivo: DialogoNodo.h

`cpp
#ifndef DIALOGO_NODO_H
#define DIALOGO_NODO_H

#include <QDialog>
#include <QString>

namespace Ui
{
    class NodeDialog;
}

class DialogoNodo : public QDialog
{
private:
    Ui::NodeDialog *ui;

public:
    explicit DialogoNodo(QWidget *parent = nullptr);

    ~DialogoNodo() override;

    QString getNombreNodo() const;

    QString getTipoNodo() const;
};

#endif // DIALOGO_NODO_H

`

### Archivo: DialogoNodo.cpp

`cpp
#include "DialogoNodo.h"

#include "ui_nodedialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QWidget>

DialogoNodo::DialogoNodo(QWidget *parent) : QDialog(parent)
{
    ui = new Ui::NodeDialog();
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Crear"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));
}

DialogoNodo::~DialogoNodo()
{
    delete ui;
    ui = nullptr;
}

QString DialogoNodo::getNombreNodo() const
{
    return ui->edtName->text().trimmed();
}

QString DialogoNodo::getTipoNodo() const
{
    int idx = ui->comboType->currentIndex();
    if (idx == 1) return "Aeropuerto";
    if (idx == 2) return "Terminal";
    if (idx == 3) return "Turistico";
    return "Normal";
}

`

### Archivo: nodedialog.ui

`xml
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>NodeDialog</class>
 <widget class="QDialog" name="NodeDialog">
  <property name="minimumSize">
   <size>
    <width>420</width>
    <height>0</height>
   </size>
  </property>
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>452</width>
    <height>238</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>Nuevo nodo</string>
  </property>
  <layout class="QVBoxLayout" name="layoutDialog">
   <property name="spacing">
    <number>16</number>
   </property>
   <property name="leftMargin">
    <number>22</number>
   </property>
   <property name="topMargin">
    <number>20</number>
   </property>
   <property name="rightMargin">
    <number>22</number>
   </property>
   <property name="bottomMargin">
    <number>20</number>
   </property>
   <item>
    <widget class="QLabel" name="lblDialogTitle">
     <property name="text">
      <string>Agregar nodo</string>
     </property>
    </widget>
   </item>
   <item>
    <layout class="QFormLayout" name="formLayout">
     <property name="horizontalSpacing">
      <number>0</number>
     </property>
     <property name="verticalSpacing">
      <number>8</number>
     </property>
     <property name="rowWrapPolicy">
      <enum>QFormLayout::WrapAllRows</enum>
     </property>
     <property name="fieldGrowthPolicy">
      <enum>QFormLayout::AllNonFixedFieldsGrow</enum>
     </property>
     <item row="0" column="0">
      <widget class="QLabel" name="lblName">
       <property name="text">
        <string>Nombre del lugar</string>
       </property>
       <property name="buddy">
        <cstring>edtName</cstring>
       </property>
      </widget>
     </item>
     <item row="0" column="1">
      <widget class="QLineEdit" name="edtName">
       <property name="minimumSize">
        <size>
         <width>0</width>
         <height>34</height>
        </size>
       </property>
       <property name="text">
        <string>Nuevo lugar</string>
       </property>
      </widget>
     </item>
     <item row="1" column="0">
      <widget class="QLabel" name="lblType">
       <property name="text">
        <string>Tipo de lugar</string>
       </property>
       <property name="buddy">
        <cstring>comboType</cstring>
       </property>
      </widget>
     </item>
     <item row="1" column="1">
      <widget class="QComboBox" name="comboType">
       <property name="minimumSize">
        <size>
         <width>0</width>
         <height>34</height>
        </size>
       </property>
       <item>
        <property name="text">
         <string>Normal</string>
        </property>
       </item>
       <item>
        <property name="text">
         <string>Aeropuerto (+$8,600)</string>
        </property>
       </item>
       <item>
        <property name="text">
         <string>Terminal de transportes (+$2,400)</string>
        </property>
       </item>
       <item>
        <property name="text">
         <string>Sitio turístico (+$2,000)</string>
        </property>
       </item>
      </widget>
     </item>
    </layout>
   </item>
   <item>
    <widget class="QDialogButtonBox" name="buttonBox">
     <property name="orientation">
      <enum>Qt::Horizontal</enum>
     </property>
     <property name="standardButtons">
      <set>QDialogButtonBox::Cancel|QDialogButtonBox::Ok</set>
     </property>
    </widget>
   </item>
  </layout>
 </widget>
 <resources />
 <connections>
  <connection>
   <sender>buttonBox</sender>
   <signal>accepted()</signal>
   <receiver>NodeDialog</receiver>
   <slot>accept()</slot>
  </connection>
  <connection>
   <sender>buttonBox</sender>
   <signal>rejected()</signal>
   <receiver>NodeDialog</receiver>
   <slot>reject()</slot>
  </connection>
 </connections>
</ui>

`
