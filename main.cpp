#include <QApplication>
#include <QChart>
#include <QChartView>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QElapsedTimer>
#include <QFontMetricsF>
#include <QHeaderView>
#include <QInputDialog>
#include <QLegend>
#include <QLineEdit>
#include <QLineF>
#include <QLineSeries>
#include <QLinearGradient>
#include <QLocale>
#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QPolygonF>
#include <QPushButton>
#include <QRandomGenerator>
#include <QRectF>
#include <QStatusBar>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QTextEdit>
#include <QTimer>
#include <QTransform>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QWidget>
#include <QtMath>
#include <cmath>

#include "ui_mainwindow.h"
#include "ui_nodedialog.h"

// =====================================================================================
//  1. CLASES DE ESTRUCTURAS DE DATOS (Nodo, Arista, ListaNodos, ListaAristas, Cola, Pila)
// =====================================================================================

class Nodo
{
private:
    int id;
    QString nombre;
    int x;
    int y;
    QString tipo; // "Normal", "Aeropuerto", "Terminal", "Turistico"

public:
    Nodo(int id, QString nombre, int x, int y, QString tipo)
    {
        this->id = id;
        this->nombre = nombre;
        this->x = x;
        this->y = y;
        this->tipo = tipo;
    }

    int getId() const { return id; }
    QString getNombre() const { return nombre; }
    int getX() const { return x; }
    int getY() const { return y; }
    QPointF getPosicion() const { return QPointF(x, y); }
    QString getTipo() const { return tipo; }

    void setId(int id) { this->id = id; }
    void setNombre(QString nombre) { this->nombre = nombre; }
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setTipo(QString tipo) { this->tipo = tipo; }
};

class Arista
{
private:
    int origen;
    int destino;
    double peso;
    double pesoInicial;
    bool esAvenida;

public:
    Arista(int origen, int destino, double peso, bool esAvenida)
    {
        this->origen = origen;
        this->destino = destino;
        this->peso = peso;
        this->pesoInicial = peso;
        this->esAvenida = esAvenida;
    }

    int getOrigen() const { return origen; }
    int getDestino() const { return destino; }
    double getPeso() const { return peso; }
    double getPesoInicial() const { return pesoInicial; }
    bool getEsAvenida() const { return esAvenida; }

    void setOrigen(int origen) { this->origen = origen; }
    void setDestino(int destino) { this->destino = destino; }
    void setPeso(double peso) { this->peso = peso; }
    void setPesoInicial(double pesoInicial) { this->pesoInicial = pesoInicial; }
    void setEsAvenida(bool esAvenida) { this->esAvenida = esAvenida; }
};

class ElementoNodo
{
private:
    Nodo *nodo;
    ElementoNodo *siguiente;

public:
    ElementoNodo(Nodo *nodo)
    {
        this->nodo = nodo;
        this->siguiente = nullptr;
    }

    Nodo *getNodo() const { return nodo; }
    ElementoNodo *getSiguiente() const { return siguiente; }
    void setSiguiente(ElementoNodo *siguiente) { this->siguiente = siguiente; }
};

class ListaNodos
{
private:
    ElementoNodo *primero;
    int cantidad;

public:
    ListaNodos()
    {
        primero = nullptr;
        cantidad = 0;
    }

    ~ListaNodos()
    {
        limpiar();
    }

    void agregar(Nodo *nodo)
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

    Nodo *buscarPorId(int id) const
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

    bool eliminarPorId(int id)
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

    ElementoNodo *getPrimero() const { return primero; }
    int getCantidad() const { return cantidad; }

    void limpiar()
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
};

class ElementoArista
{
private:
    Arista *arista;
    ElementoArista *siguiente;

public:
    ElementoArista(Arista *arista)
    {
        this->arista = arista;
        this->siguiente = nullptr;
    }

    Arista *getArista() const { return arista; }
    ElementoArista *getSiguiente() const { return siguiente; }
    void setSiguiente(ElementoArista *siguiente) { this->siguiente = siguiente; }
};

class ListaAristas
{
private:
    ElementoArista *primero;
    int cantidad;

public:
    ListaAristas()
    {
        primero = nullptr;
        cantidad = 0;
    }

    ~ListaAristas()
    {
        limpiar();
    }

    void agregar(Arista *arista)
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

    Arista *obtenerArista(int origen, int destino) const
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

    bool existeArista(int origen, int destino) const
    {
        return obtenerArista(origen, destino) != nullptr;
    }

    bool eliminarArista(int origen, int destino)
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

    void eliminarAristasDeNodo(int nodoId)
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

    ElementoArista *getPrimero() const { return primero; }
    int getCantidad() const { return cantidad; }

    void limpiar()
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
};

class NodoCola
{
private:
    int dato;
    NodoCola *siguiente;

public:
    NodoCola(int dato)
    {
        this->dato = dato;
        this->siguiente = nullptr;
    }

    int getDato() const { return dato; }
    NodoCola *getSiguiente() const { return siguiente; }
    void setSiguiente(NodoCola *siguiente) { this->siguiente = siguiente; }
};

class Cola
{
private:
    NodoCola *frente;
    NodoCola *finalCola;

public:
    Cola()
    {
        frente = nullptr;
        finalCola = nullptr;
    }

    ~Cola()
    {
        limpiar();
    }

    void encolar(int dato)
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

    int desencolar()
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

    bool estaVacia() const
    {
        return frente == nullptr;
    }

    void limpiar()
    {
        while (!estaVacia())
        {
            desencolar();
        }
    }
};

class NodoPila
{
private:
    int dato;
    NodoPila *siguiente;

public:
    NodoPila(int dato)
    {
        this->dato = dato;
        this->siguiente = nullptr;
    }

    int getDato() const { return dato; }
    NodoPila *getSiguiente() const { return siguiente; }
    void setSiguiente(NodoPila *siguiente) { this->siguiente = siguiente; }
};

class Pila
{
private:
    NodoPila *cima;

public:
    Pila()
    {
        cima = nullptr;
    }

    ~Pila()
    {
        limpiar();
    }

    void apilar(int dato)
    {
        NodoPila *nuevo = new NodoPila(dato);
        nuevo->setSiguiente(cima);
        cima = nuevo;
    }

    int desapilar()
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

    bool estaVacia() const
    {
        return cima == nullptr;
    }

    void limpiar()
    {
        while (!estaVacia())
        {
            desapilar();
        }
    }
};

// =====================================================================================
//  2. CLASES DE MODELO (ResultadoRuta, Pasajero, ManzanaCiudad, TextoFlotante)
// =====================================================================================

class ResultadoRuta
{
private:
    bool encontrada;
    int camino[100];
    int cantidadNodos;
    double tiempoPonderado;
    int nodosExplorados;

public:
    ResultadoRuta()
    {
        encontrada = false;
        cantidadNodos = 0;
        tiempoPonderado = 0.0;
        nodosExplorados = 0;
    }

    bool getEncontrada() const { return encontrada; }
    int getCantidadNodos() const { return cantidadNodos; }
    int getNodoEn(int indice) const { return camino[indice]; }
    double getTiempoPonderado() const { return tiempoPonderado; }
    int getNodosExplorados() const { return nodosExplorados; }

    int getSaltos() const
    {
        if (cantidadNodos <= 1)
        {
            return 0;
        }
        return cantidadNodos - 1;
    }

    void setEncontrada(bool encontrada) { this->encontrada = encontrada; }
    void setTiempoPonderado(double tiempoPonderado) { this->tiempoPonderado = tiempoPonderado; }
    void setNodosExplorados(int nodosExplorados) { this->nodosExplorados = nodosExplorados; }

    void limpiarCamino()
    {
        cantidadNodos = 0;
    }

    void agregarNodoAlCamino(int idNodo)
    {
        if (cantidadNodos < 100)
        {
            camino[cantidadNodos] = idNodo;
            cantidadNodos++;
        }
    }
};

class Pasajero
{
private:
    int id;
    int origen;
    int destino;
    int idTaxiAsignado;
    double tarifa;

public:
    Pasajero(int id, int origen, int destino, double tarifa)
    {
        this->id = id;
        this->origen = origen;
        this->destino = destino;
        this->idTaxiAsignado = -1;
        this->tarifa = tarifa;
    }

    int getId() const { return id; }
    int getOrigen() const { return origen; }
    int getDestino() const { return destino; }
    int getIdTaxiAsignado() const { return idTaxiAsignado; }
    double getTarifa() const { return tarifa; }
    bool estaReservado() const { return idTaxiAsignado >= 0; }

    void setIdTaxiAsignado(int idTaxi) { this->idTaxiAsignado = idTaxi; }
    void setTarifa(double tarifa) { this->tarifa = tarifa; }
};

class ManzanaCiudad
{
private:
    QPolygonF poligono;
    QString tipo;

public:
    ManzanaCiudad(QPolygonF poligono, QString tipo)
    {
        this->poligono = poligono;
        this->tipo = tipo;
    }

    QPolygonF getPoligono() const { return poligono; }
    QString getTipo() const { return tipo; }
};

class TextoFlotante
{
private:
    QPointF posicion;
    QString texto;
    QColor color;
    double edad;

public:
    TextoFlotante(QPointF posicion, QString texto, QColor color)
    {
        this->posicion = posicion;
        this->texto = texto;
        this->color = color;
        this->edad = 0.0;
    }

    QPointF getPosicion() const { return posicion; }
    QString getTexto() const { return texto; }
    QColor getColor() const { return color; }
    double getEdad() const { return edad; }
    void avanzarEdad(double dt) { edad += dt; }
};

// =====================================================================================
//  3. CLASE GRAFO (Grafo Dirigido y Ponderado con BFS, DFS, Dijkstra y Taxímetro)
// =====================================================================================

class Grafo : public QObject
{
    Q_OBJECT

private:
    ListaNodos *nodos;
    ListaAristas *aristas;
    double multiplicadorHoraPico;

    ResultadoRuta reconstruirRuta(int anterior[], int origen, int destino, int explorados) const
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

public:
    explicit Grafo(QObject *parent = nullptr) : QObject(parent)
    {
        nodos = new ListaNodos();
        aristas = new ListaAristas();
        multiplicadorHoraPico = 1.0;
    }

    ~Grafo() override
    {
        delete nodos;
        delete aristas;
        nodos = nullptr;
        aristas = nullptr;
    }

    ListaNodos *getNodos() const { return nodos; }
    ListaAristas *getAristas() const { return aristas; }

    Nodo *buscarNodo(int id) const
    {
        return nodos->buscarPorId(id);
    }

    bool tieneNodo(int id) const
    {
        return buscarNodo(id) != nullptr;
    }

    int obtenerSiguienteIdDisponible() const
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

    int agregarNodo(int id, QString nombre, int x, int y, QString tipo)
    {
        if (id < 0 || id >= 100 || buscarNodo(id) != nullptr)
        {
            return -1;
        }

        nodos->agregar(new Nodo(id, nombre, x, y, tipo));
        emit grafoModificado();
        return id;
    }

    int agregarNodoNuevo(QString nombre, QPointF posicion, QString tipo)
    {
        int id = obtenerSiguienteIdDisponible();
        if (id < 0)
        {
            return -1;
        }
        return agregarNodo(id, nombre, static_cast<int>(posicion.x()), static_cast<int>(posicion.y()), tipo);
    }

    bool eliminarNodo(int id)
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

    double sugerirPeso(int nodoA, int nodoB, bool esAvenida) const
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

    bool agregarArista(int origen, int destino, double peso = -1.0, bool esAvenida = false)
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

    bool agregarCalleDoble(int nodoA, int nodoB, double peso = -1.0, bool esAvenida = false)
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

    bool eliminarCalle(int nodoA, int nodoB)
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

    bool cambiarPeso(int nodoA, int nodoB, double nuevoPeso)
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

    void limpiar()
    {
        nodos->limpiar();
        aristas->limpiar();
        emit grafoModificado();
    }

    Arista *obtenerArista(int origen, int destino) const
    {
        return aristas->obtenerArista(origen, destino);
    }

    bool existeAristaDirigida(int origen, int destino) const
    {
        return aristas->existeArista(origen, destino);
    }

    bool tieneSalidas(int idNodo) const
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

    double obtenerPesoEfectivo(int origen, int destino) const
    {
        Arista *a = aristas->obtenerArista(origen, destino);
        if (a == nullptr)
        {
            return 0.0;
        }
        return a->getPeso() * multiplicadorHoraPico;
    }

    double obtenerPesoEfectivoArista(Arista *a) const
    {
        if (a == nullptr)
        {
            return 0.0;
        }
        return a->getPeso() * multiplicadorHoraPico;
    }

    double obtenerNivelCongestion(Arista *a) const
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

    /** Requerimiento: El peso de cada arista aumenta por uso. */
    void registrarUso(int origen, int destino)
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

    int nodoMasCercano(QPointF punto, double distanciaMaxima = 1e9) const
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

    void setMultiplicadorHoraPico(double multiplicador)
    {
        this->multiplicadorHoraPico = multiplicador;
    }

    double getMultiplicadorHoraPico() const
    {
        return multiplicadorHoraPico;
    }

    // ---- Algoritmo 1: Búsqueda en Anchura (BFS) usando la clase Cola ----
    ResultadoRuta buscarRutaBFS(int origen, int destino) const
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

    // ---- Algoritmo 2: Búsqueda en Profundidad (DFS) usando la clase Pila ----
    ResultadoRuta buscarRutaDFS(int origen, int destino) const
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

    // ---- Algoritmo 3: Algoritmo de Dijkstra (camino de menor peso ponderado) ----
    ResultadoRuta buscarRutaDijkstra(int origen, int destino) const
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

    /**
     * Fórmula del Taxímetro:
     *   Taxímetro = Base ($4,600) + Distancia (salto) x $1,625 + Tiempo (ponderado) x $1,625
     *   Mínimo = $8,100
     *   Recargos: Aeropuerto (+$8,600), Terminal (+$2,400), Turístico (+$2,000), Nocturno (+$2,000)
     */
    static double calcularTarifa(int saltos, double tiempoPonderado, QString tipoDestino, QString periodo)
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

    static QString formatearPesos(double valor)
    {
        qint64 redondeado = qRound64(valor);
        return QStringLiteral("$ ") + QLocale(QLocale::English).toString(qAbs(redondeado));
    }

signals:
    void grafoModificado();
};

// =====================================================================================
//  4. CLASE RELOJ DE SIMULACIÓN
// =====================================================================================

class RelojSimulacion
{
private:
    double hora;
    int dia;
    int modo; // 0 = Automático, 1 = Normal, 2 = Nocturno, 3 = Hora Pico

public:
    RelojSimulacion()
    {
        hora = 6.0;
        dia = 1;
        modo = 0;
    }

    void reiniciar()
    {
        hora = 6.0;
        dia = 1;
    }

    void avanzar(double segundosSimulados)
    {
        hora += segundosSimulados * (24.0 / 240.0);
        while (hora >= 24.0)
        {
            hora -= 24.0;
            dia++;
        }
    }

    int getDia() const { return dia; }
    int getModo() const { return modo; }
    void setModo(int modo) { this->modo = modo; }

    QString textoHora() const
    {
        int h = static_cast<int>(hora);
        int m = static_cast<int>((hora - h) * 60.0);
        return QStringLiteral("%1:%2")
            .arg(h, 2, 10, QLatin1Char('0'))
            .arg(m, 2, 10, QLatin1Char('0'));
    }

    QString periodoActual() const
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

    double oscuridad() const
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
};

// =====================================================================================
//  5. JERARQUÍA DE TAXIS (Taxi -> TaxiBFS, TaxiDFS, TaxiDijkstra, TaxiManual)
// =====================================================================================

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
    Taxi(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
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

    virtual ~Taxi() {}

    virtual QString nombreAlgoritmo() const = 0;
    virtual bool esManual() const { return false; }
    virtual ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino)
    {
        Q_UNUSED(grafo);
        Q_UNUSED(origen);
        Q_UNUSED(destino);
        return ResultadoRuta();
    }

    int getId() const { return id; }
    QString getNombre() const { return nombre; }
    QColor getColor() const { return color; }
    int getNodoActual() const { return nodoActual; }
    QPointF getPosicion() const { return posicion; }
    double getAngulo() const { return angulo; }
    bool getEnMovimiento() const { return enMovimiento; }
    int getNodoSiguiente() const { return nodoSiguiente; }
    int getCantidadRuta() const { return cantidadRuta; }
    int getPasoRuta(int indice) const { return ruta[indice]; }
    bool getLlevaPasajero() const { return llevaPasajero; }
    int getDestinoPasajeroABordo() const { return destinoPasajeroABordo; }
    double getTarifaPasajeroABordo() const { return tarifaPasajeroABordo; }
    int getIdPasajeroObjetivo() const { return idPasajeroObjetivo; }
    int getViajes() const { return viajes; }
    double getIngresos() const { return ingresos; }
    long getNodosExplorados() const { return nodosExplorados; }
    double getEsperaPensar() const { return esperaPensar; }

    void setNodoActual(int idNodo) { this->nodoActual = idNodo; }
    void setPosicion(QPointF pos) { this->posicion = pos; }
    void setIdPasajeroObjetivo(int idPasajero) { this->idPasajeroObjetivo = idPasajero; }
    void setEsperaPensar(double espera) { this->esperaPensar = espera; }
    void sumarExplorados(int cantidad) { this->nodosExplorados += cantidad; }

    void subirPasajero(int destino, double tarifa)
    {
        llevaPasajero = true;
        destinoPasajeroABordo = destino;
        tarifaPasajeroABordo = tarifa;
        idPasajeroObjetivo = -1;
    }

    void bajarPasajero()
    {
        ingresos += tarifaPasajeroABordo;
        viajes++;
        llevaPasajero = false;
        destinoPasajeroABordo = -1;
        tarifaPasajeroABordo = 0.0;
        idPasajeroObjetivo = -1;
    }

    void limpiarRuta()
    {
        cantidadRuta = 0;
    }

    void agregarPasoARuta(int idNodo)
    {
        if (cantidadRuta < 100)
        {
            ruta[cantidadRuta] = idNodo;
            cantidadRuta++;
        }
    }

    void cargarRutaDesdeResultado(const ResultadoRuta &res)
    {
        limpiarRuta();
        for (int i = 1; i < res.getCantidadNodos(); i++)
        {
            agregarPasoARuta(res.getNodoEn(i));
        }
    }

    int quitarPrimerPasoRuta()
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

    int nodoFinalDeRuta() const
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

    void iniciarTramo(Grafo *grafo)
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

    bool avanzarEnTramo(double dt, Grafo *grafo)
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

    void suavizarAngulo(double dt)
    {
        double dif = std::fmod(anguloObjetivo - angulo + 540.0, 360.0) - 180.0;
        double factor = dt * 10.0;
        if (factor > 1.0)
        {
            factor = 1.0;
        }
        angulo += dif * factor;
    }

    QString textoEstado(Grafo *grafo) const
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
};

class TaxiBFS : public Taxi
{
public:
    TaxiBFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
        : Taxi(id, nombre, color, nodoInicial, grafo)
    {
    }

    QString nombreAlgoritmo() const override
    {
        return QStringLiteral("BFS");
    }

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override
    {
        ResultadoRuta res = grafo->buscarRutaBFS(origen, destino);
        sumarExplorados(res.getNodosExplorados());
        return res;
    }
};

class TaxiDFS : public Taxi
{
public:
    TaxiDFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
        : Taxi(id, nombre, color, nodoInicial, grafo)
    {
    }

    QString nombreAlgoritmo() const override
    {
        return QStringLiteral("DFS");
    }

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override
    {
        ResultadoRuta res = grafo->buscarRutaDFS(origen, destino);
        sumarExplorados(res.getNodosExplorados());
        return res;
    }
};

class TaxiDijkstra : public Taxi
{
public:
    TaxiDijkstra(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
        : Taxi(id, nombre, color, nodoInicial, grafo)
    {
    }

    QString nombreAlgoritmo() const override
    {
        return QStringLiteral("Dijkstra");
    }

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override
    {
        ResultadoRuta res = grafo->buscarRutaDijkstra(origen, destino);
        sumarExplorados(res.getNodosExplorados());
        return res;
    }
};

class TaxiManual : public Taxi
{
public:
    TaxiManual(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
        : Taxi(id, nombre, color, nodoInicial, grafo)
    {
    }

    QString nombreAlgoritmo() const override
    {
        return QStringLiteral("Manual");
    }

    bool esManual() const override
    {
        return true;
    }

    bool agregarNodoARuta(int idNodo, Grafo *grafo)
    {
        int desde = nodoFinalDeRuta();
        if (grafo == nullptr || !grafo->existeAristaDirigida(desde, idNodo))
        {
            return false;
        }
        agregarPasoARuta(idNodo);
        return true;
    }
};

// =====================================================================================
//  6. CLASE SIMULACIÓN
// =====================================================================================

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

    void agregarManzana(int n1, int n2, int n3, int n4, QString tipoManzana)
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

    void construirCiudadInicial()
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

    void liberarTaxis()
    {
        for (int i = 0; i < cantidadTaxis; i++)
        {
            delete taxis[i];
            taxis[i] = nullptr;
        }
        cantidadTaxis = 0;
        taxiManual = nullptr;
    }

    void crearTaxis()
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

    void generarPasajero()
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

    int buscarIndicePasajeroPorId(int idPasajero) const
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

    int asignarPasajeroATaxi(Taxi *taxi)
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

    void revisarParadaTaxi(Taxi *taxi)
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

    void planificarRutaAutonoma(Taxi *taxi)
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

    void actualizarTaxi(Taxi *taxi, double dt)
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

    void avanzarUnPaso(double dt)
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

    void actualizarPeriodo(bool silencioso)
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

    QString nombreNodo(int id) const
    {
        Nodo *n = grafo->buscarNodo(id);
        return (n != nullptr) ? n->getNombre() : QStringLiteral("?");
    }

    void registrarEvento(QString texto, QColor color)
    {
        emit mensajeEvento(QStringLiteral("[%1] %2").arg(reloj.textoHora(), texto), color);
    }

private slots:
    void enTickReloj()
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

public:
    explicit Simulacion(QObject *parent = nullptr) : QObject(parent)
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

    ~Simulacion() override
    {
        liberarTaxis();
    }

    Grafo *getGrafo() const { return grafo; }
    const RelojSimulacion &getReloj() const { return reloj; }
    int getCantidadTaxis() const { return cantidadTaxis; }
    Taxi *getTaxi(int indice) const { return taxis[indice]; }
    TaxiManual *getTaxiManual() const { return taxiManual; }
    const QVector<Pasajero> &getPasajerosEnEspera() const { return pasajeros; }
    QRectF getLimitesMapa() const { return limitesMapa; }
    QPolygonF getPoligonoAgua() const { return agua; }
    const QVector<ManzanaCiudad> &getManzanas() const { return manzanas; }

    void iniciar()
    {
        relojCuadro.start();
        temporizador.start();
    }

    bool estaPausada() const { return pausada; }

    void setPausada(bool p)
    {
        if (pausada == p)
        {
            return;
        }
        pausada = p;
        emit pausaCambiada(pausada);
    }

    void setVelocidad(double mult)
    {
        velocidad = mult;
    }

    void setModoTiempo(int modo)
    {
        reloj.setModo(modo);
        actualizarPeriodo(false);
    }

    void reiniciar()
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

    double cotizarCarrera(int origen, int destino) const
    {
        ResultadoRuta r = grafo->buscarRutaDijkstra(origen, destino);
        Nodo *nodoDest = grafo->buscarNodo(destino);
        QString tipoDest = (nodoDest != nullptr) ? nodoDest->getTipo() : QStringLiteral("Normal");
        return Grafo::calcularTarifa(r.getSaltos(), r.getTiempoPonderado(), tipoDest, reloj.periodoActual());
    }

signals:
    void cuadroActualizado(double dtReal);
    void muestraGrafico(double segundosSimulados);
    void reinicioCompletado();
    void pausaCambiada(bool pausada);
    void mensajeEvento(QString texto, QColor color);
    void carreraCompletada(int idTaxi, QPointF posMapa, double tarifa, QColor color);
};

// =====================================================================================
//  7. INTERFAZ GRÁFICA (MapaWidget, BarrasGananciasWidget, GraficoGananciasWidget, MainWindow)
// =====================================================================================

class MapaWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    QElapsedTimer relojAnimacion;
    double oscuridad;
    int nodoBajoMouse;
    int nodoPendiente;
    bool modoConducir;
    bool mostrarPesos;
    QVector<TextoFlotante> textosFlotantes;

    static QFont fuentePixel(int px, bool negrita = false)
    {
        QFont f(QStringLiteral("Segoe UI"));
        f.setPixelSize(px);
        f.setBold(negrita);
        return f;
    }

    static QColor obtenerColorTipoNodo(QString tipo)
    {
        if (tipo == "Aeropuerto")
        {
            return QColor("#2f80ed");
        }
        if (tipo == "Terminal")
        {
            return QColor("#f2994a");
        }
        if (tipo == "Turistico")
        {
            return QColor("#f2c94c");
        }
        return QColor("#eef1f6");
    }

    static QColor mezclarColores(QColor a, QColor b, double t)
    {
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;
        return QColor::fromRgbF(a.redF() + (b.redF() - a.redF()) * t,
                                a.greenF() + (b.greenF() - a.greenF()) * t,
                                a.blueF() + (b.blueF() - a.blueF()) * t);
    }

    static void dibujarTextoConBorde(QPainter &p, QPointF centro, QString texto,
                                     QFont fuente, QColor relleno, double grosorBorde = 3.5)
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

    QTransform transformacionVista() const
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

    QPointF posicionDibujoTaxi(const Taxi *taxi) const
    {
        double rad = qDegreesToRadians(taxi->getAngulo());
        return taxi->getPosicion() + QPointF(-std::sin(rad), std::cos(rad)) * 6.0;
    }

    void dibujarMundo(QPainter &p, bool esMinimapa)
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
                double alfa = 1.0 - k * k;
                if (alfa < 0.0) alfa = 0.0;
                if (alfa > 1.0) alfa = 1.0;
                c.setAlpha(static_cast<int>(255 * alfa));
                dibujarTextoConBorde(p, tf.getPosicion() + QPointF(0, -50 - 55 * k),
                                     tf.getTexto(), fuentePixel(26, true), c, 4.5);
            }
        }
    }

    void dibujarEscenario(QPainter &p)
    {
        QRectF b = sim->getLimitesMapa();
        QLinearGradient fondo(b.topLeft(), b.bottomRight());
        fondo.setColorAt(0.0, QColor("#22304a"));
        fondo.setColorAt(1.0, QColor("#1b2639"));
        p.fillRect(b, fondo);

        p.setPen(QPen(QColor("#2d79b3"), 6));
        p.setBrush(QColor("#14395e"));
        p.drawPolygon(sim->getPoligonoAgua());

        const QVector<ManzanaCiudad> &manzanas = sim->getManzanas();
        for (int i = 0; i < manzanas.size(); i++)
        {
            const ManzanaCiudad &m = manzanas[i];
            QColor color = QColor("#2f4160");
            if (m.getTipo() == "Parque")
            {
                color = QColor("#2e6b4a");
            }
            else if (m.getTipo() == "Industrial")
            {
                color = QColor("#51483c");
            }
            else if (m.getTipo() == "Aeropuerto")
            {
                color = QColor("#3b4d6b");
            }
            else if (m.getTipo() == "Terminal")
            {
                color = QColor("#4d4363");
            }

            p.setPen(QPen(color, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(color);
            p.drawPolygon(m.getPoligono());
        }
    }

    void dibujarCalles(QPainter &p, bool esMinimapa)
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
            p.setPen(QPen(QColor("#0e141f"), av ? 25 : 19, Qt::SolidLine, Qt::RoundCap));
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
            QColor base = av ? QColor("#e9edf5") : QColor("#9fb0c7");
            QColor c = mezclarColores(base, QColor("#ff4a3d"),
                                      (g->obtenerNivelCongestion(arista) - 1.05) / 2.0);
            p.setPen(QPen(c, av ? 19 : 13, Qt::SolidLine, Qt::RoundCap));
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
            QLineF linea(a, b);
            if (tieneInversa)
            {
                p.setPen(QPen(QColor(40, 50, 68, 150), 1.6, Qt::DashLine));
                p.drawLine(linea);
            }
            else
            {
                double lon = linea.length();
                if (lon < 1.0)
                {
                    continue;
                }
                QPointF d = (b - a) / lon;
                QPointF n(-d.y(), d.x());
                QPointF c = (a + b) / 2.0;
                QPointF pts[3] = {c - d * 5.0 + n * 5.0, c + d * 5.0, c - d * 5.0 - n * 5.0};
                p.setPen(QPen(QColor(38, 48, 66), 2.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                p.drawPolyline(pts, 3);
            }
        }
    }

    void dibujarRutas(QPainter &p)
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

            QColor c = taxi->getColor();
            c.setAlpha(taxi->esManual() ? 220 : 130);
            p.setPen(QPen(c, taxi->esManual() ? 6 : 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(Qt::NoBrush);
            p.drawPolyline(poli);
        }
    }

    void dibujarPesos(QPainter &p)
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
            QColor tono = mezclarColores(QColor(235, 240, 250), QColor("#ff4a3d"),
                                         (g->obtenerNivelCongestion(a) - 1.05) / 1.6);
            double ancho = fm.horizontalAdvance(texto) + 10;
            QRectF caja(centro.x() - ancho / 2, centro.y() - 8, ancho, 16);
            p.setPen(QPen(tono, 1.6));
            p.setBrush(QColor(10, 14, 24, 232));
            p.drawRoundedRect(caja, 6, 6);
            p.setPen(tono);
            p.drawText(caja, Qt::AlignCenter, texto);
        }
    }

    void dibujarNodos(QPainter &p, bool esMinimapa)
    {
        Grafo *g = sim->getGrafo();
        double t = relojAnimacion.elapsed() / 1000.0;

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

            p.setPen(QPen(especial ? QColor(255, 255, 255) : QColor("#0e141f"), 3.2));
            p.setBrush(colorTipo);
            p.drawEllipse(n->getPosicion(), radio, radio);

            if (especial)
            {
                QString simbolo = QStringLiteral("★");
                if (n->getTipo() == "Aeropuerto")
                {
                    simbolo = QStringLiteral("✈");
                }
                else if (n->getTipo() == "Terminal")
                {
                    simbolo = QStringLiteral("▣");
                }
                p.setFont(fuentePixel(esMinimapa ? 13 : 16, true));
                p.setPen(n->getTipo() == "Turistico" ? QColor(65, 38, 0) : QColor(255, 255, 255));
                p.drawText(QRectF(n->getPosicion().x() - radio, n->getPosicion().y() - radio,
                                  radio * 2, radio * 2),
                           Qt::AlignCenter, simbolo);
            }

            if (!esMinimapa)
            {
                bool resaltado = (n->getId() == nodoBajoMouse);
                QFont f = fuentePixel(especial ? 15 : 13, especial || resaltado);
                QColor c = resaltado ? QColor(255, 255, 255)
                                     : (especial ? colorTipo : QColor(200, 212, 230));
                dibujarTextoConBorde(p, n->getPosicion() + QPointF(0, especial ? 34 : 26),
                                     n->getNombre(), f, c);
            }
        }

        if (esMinimapa)
        {
            return;
        }

        if (g->buscarNodo(nodoPendiente) != nullptr)
        {
            p.setPen(QPen(QColor("#ff9f1c"), 4));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(g->buscarNodo(nodoPendiente)->getPosicion(),
                          26 + 3 * std::sin(t * 6), 26 + 3 * std::sin(t * 6));
        }

        if (modoConducir && sim->getTaxiManual() != nullptr)
        {
            double r = 24 + 3 * std::sin(t * 5);
            p.setPen(QPen(QColor(255, 214, 10, 230), 3, Qt::DashLine));
            p.setBrush(QColor(255, 214, 10, 40));

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

    void dibujarPasajeros(QPainter &p, bool esMinimapa)
    {
        Grafo *g = sim->getGrafo();
        double t = relojAnimacion.elapsed() / 1000.0;
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
            QColor colorAro("#5eead4");

            if (!libre)
            {
                for (int k = 0; k < sim->getCantidadTaxis(); k++)
                {
                    Taxi *tx = sim->getTaxi(k);
                    if (tx->getId() == pas.getIdTaxiAsignado())
                    {
                        colorAro = tx->getColor();
                    }
                }
            }

            double r = libre ? 26 + 4 * std::sin(t * 5 + pas.getId()) : 18;
            p.setPen(QPen(colorAro, libre ? 4.0 : 2.5));
            p.setBrush(QColor(colorAro.red(), colorAro.green(), colorAro.blue(), libre ? 70 : 30));
            p.drawEllipse(o, r, r);

            double salto = libre ? -4.0 * std::abs(std::sin(t * 3 + pas.getId())) : 0.0;
            p.save();
            p.translate(o + QPointF(0, -28 + salto));
            p.setPen(QPen(QColor(8, 12, 20), 2));
            p.setBrush(colorAro);
            p.drawEllipse(QPointF(0, -6), 5.0, 5.0);
            p.drawRoundedRect(QRectF(-6, 0, 12, 10), 4, 4);
            p.restore();

            if (libre && !esMinimapa)
            {
                QString texto = QStringLiteral("Pasajero → %1  ·  %2")
                                    .arg(nd->getNombre(),
                                         Grafo::formatearPesos(sim->cotizarCarrera(pas.getOrigen(), pas.getDestino())));
                QFont f = fuentePixel(12, true);
                QFontMetricsF fm(f);
                double w = fm.horizontalAdvance(texto) + 14;
                QRectF globo(o.x() - w / 2, o.y() - 64 + salto, w, 20);
                p.setPen(QPen(colorAro, 1.8));
                p.setBrush(QColor(8, 12, 20, 235));
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
            p.setBrush(taxi->getColor());
            p.drawPolygon(rombo);
            p.restore();

            if (taxi->esManual() && !esMinimapa)
            {
                dibujarTextoConBorde(p, d + QPointF(0, -54), QStringLiteral("DESTINO"),
                                     fuentePixel(14, true), taxi->getColor());
            }
        }
    }

    void dibujarTaxis(QPainter &p, bool esMinimapa)
    {
        for (int i = 0; i < sim->getCantidadTaxis(); i++)
        {
            Taxi *taxi = sim->getTaxi(i);
            QPointF pos = posicionDibujoTaxi(taxi);

            p.save();
            p.translate(pos);
            p.rotate(taxi->getAngulo());
            p.setPen(QPen(QColor(10, 14, 22), 2.2));
            p.setBrush(taxi->getColor());
            p.drawRoundedRect(QRectF(-14, -7.5, 28, 15), 5, 5);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(10, 14, 22, 215));
            p.drawRect(QRectF(2, -5.5, 6, 11));
            p.drawRect(QRectF(-11, -5, 4, 10));
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
            p.setPen(QPen(taxi->getColor(), 1.6));
            p.setBrush(QColor(8, 12, 20, 225));
            p.drawRoundedRect(caja, 6, 6);
            p.setFont(f);
            p.setPen(QColor(255, 255, 255));
            p.drawText(caja, Qt::AlignCenter, etiqueta);
        }
    }

    void dibujarEfectosNocturnos(QPainter &p)
    {
        if (oscuridad < 0.01)
        {
            return;
        }

        p.fillRect(sim->getLimitesMapa(), QColor(6, 12, 46, static_cast<int>(150 * oscuridad)));

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

    void dibujarHUD(QPainter &p)
    {
        const RelojSimulacion &reloj = sim->getReloj();
        QString periodo = reloj.periodoActual();

        QColor colorPeriodo("#4ade80");
        QString textoPeriodo = QStringLiteral("TRÁFICO NORMAL");
        if (periodo == "HoraPico")
        {
            colorPeriodo = QColor("#ff5252");
            textoPeriodo = QStringLiteral("HORA PICO ×1.8");
        }
        else if (periodo == "Nocturno")
        {
            colorPeriodo = QColor("#a78bfa");
            textoPeriodo = QStringLiteral("NOCTURNO +$ 2,000");
        }

        QRectF panelReloj(14, 14, 216, 124);
        p.setPen(QPen(QColor(255, 255, 255, 28), 1));
        p.setBrush(QColor(8, 12, 20, 218));
        p.drawRoundedRect(panelReloj, 14, 14);

        p.setFont(fuentePixel(42, true));
        p.setPen(QColor(255, 255, 255));
        p.drawText(QRectF(30, 18, 190, 52), Qt::AlignLeft | Qt::AlignVCenter, reloj.textoHora());

        p.setFont(fuentePixel(12, true));
        p.setPen(QColor("#93a4bd"));
        p.drawText(QRectF(32, 70, 190, 16), Qt::AlignLeft | Qt::AlignVCenter,
                   QStringLiteral("DÍA %1").arg(reloj.getDia()));

        QRectF insignia(28, 96, 188, 28);
        p.setPen(QPen(colorPeriodo, 1.8));
        p.setBrush(QColor(colorPeriodo.red(), colorPeriodo.green(), colorPeriodo.blue(), 50));
        p.drawRoundedRect(insignia, 9, 9);
        p.setFont(fuentePixel(13, true));
        p.setPen(colorPeriodo);
        p.drawText(insignia, Qt::AlignCenter, textoPeriodo);

        TaxiManual *yo = sim->getTaxiManual();
        if (yo == nullptr)
        {
            return;
        }

        QRectF panelDinero(width() - 334, 14, 320, 124);
        p.setPen(QPen(QColor(255, 255, 255, 28), 1));
        p.setBrush(QColor(8, 12, 20, 218));
        p.drawRoundedRect(panelDinero, 14, 14);

        p.setFont(fuentePixel(12, true));
        p.setPen(QColor("#93a4bd"));
        p.drawText(QRectF(panelDinero.left(), 22, panelDinero.width(), 16), Qt::AlignCenter,
                   QStringLiteral("TUS INGRESOS ACUMULADOS"));

        dibujarTextoConBorde(p, QPointF(panelDinero.center().x(), 76),
                             Grafo::formatearPesos(yo->getIngresos()), fuentePixel(40, true),
                             QColor("#7dff8a"), 5.0);

        p.setFont(fuentePixel(12));
        p.setPen(QColor("#dbe3ef"));
        p.drawText(QRectF(panelDinero.left() + 12, 86, panelDinero.width() - 24, 18),
                   Qt::AlignCenter, yo->textoEstado(sim->getGrafo()));
        p.setPen(QColor("#93a4bd"));
        p.drawText(QRectF(panelDinero.left() + 12, 104, panelDinero.width() - 24, 18),
                   Qt::AlignCenter,
                   QStringLiteral("Carreras completadas: %1").arg(yo->getViajes()));
    }

    void dibujarMinimapa(QPainter &p)
    {
        TaxiManual *yo = sim->getTaxiManual();
        if (yo == nullptr || width() < 700 || height() < 480)
        {
            return;
        }

        double radio = 96.0;
        QPointF centro(14 + radio, height() - 14 - radio);

        p.save();
        QPainterPath recorte;
        recorte.addEllipse(centro, radio, radio);
        p.setClipPath(recorte);
        p.fillRect(rect(), QColor("#0b1018"));

        QTransform t;
        t.translate(centro.x(), centro.y());
        t.scale(0.34, 0.34);
        t.translate(-yo->getPosicion().x(), -yo->getPosicion().y());
        p.setTransform(t);
        dibujarMundo(p, true);
        p.restore();

        p.setPen(QPen(QColor(255, 255, 255, 230), 3));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(centro, radio, radio);
    }

private slots:
    void enCuadro(double dt)
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

    void enCarreraCompletada(int, QPointF pos, double tarifa, QColor color)
    {
        textosFlotantes.append(
            TextoFlotante(pos, QStringLiteral("+") + Grafo::formatearPesos(tarifa), color));
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::TextAntialiasing);
        p.fillRect(rect(), QColor("#0b1018"));
        if (sim == nullptr)
        {
            return;
        }

        p.save();
        p.setTransform(transformacionVista());
        p.setClipRect(sim->getLimitesMapa());
        dibujarMundo(p, false);
        p.restore();

        dibujarHUD(p);
        dibujarMinimapa(p);
    }

    void mousePressEvent(QMouseEvent *e) override
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

    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (sim == nullptr)
        {
            return;
        }
        QPointF posMapa = transformacionVista().inverted().map(QPointF(e->pos()));
        nodoBajoMouse = sim->getGrafo()->nodoMasCercano(posMapa, 30.0);
        setCursor(nodoBajoMouse >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }

    void leaveEvent(QEvent *) override
    {
        nodoBajoMouse = -1;
    }

public:
    explicit MapaWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        sim = nullptr;
        oscuridad = 0.0;
        nodoBajoMouse = -1;
        nodoPendiente = -1;
        modoConducir = true;
        mostrarPesos = true;

        setMouseTracking(true);
        setMinimumSize(640, 460);
        relojAnimacion.start();
    }

    void setSimulacion(Simulacion *nuevaSim)
    {
        sim = nuevaSim;
        connect(sim, &Simulacion::cuadroActualizado, this, &MapaWidget::enCuadro);
        connect(sim, &Simulacion::carreraCompletada, this, &MapaWidget::enCarreraCompletada);
    }

    void setNodoPendiente(int idNodo) { nodoPendiente = idNodo; }
    void setModoConducir(bool activo) { modoConducir = activo; }
    void setMostrarPesos(bool mostrar) { mostrarPesos = mostrar; }

signals:
    void nodoClicado(int idNodo);
    void espacioVacioClicado(QPointF posMapa);
    void cancelarSolicitado();
};

class BarrasGananciasWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    double mostrado[7];
    double escala;

private slots:
    void reiniciarBarras()
    {
        for (int i = 0; i < 7; i++)
        {
            mostrado[i] = 0.0;
        }
        escala = 50000.0;
    }

    void enCuadro(double dt)
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

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#0c121b"));
        p.drawRoundedRect(rect(), 10, 10);

        if (sim == nullptr || sim->getCantidadTaxis() == 0)
        {
            return;
        }

        int n = sim->getCantidadTaxis();
        double margen = 12.0;
        double anchoEtiqueta = 92.0;
        double anchoValor = 98.0;
        double altoFila = (height() - 2 * margen) / n;
        double altoBarra = altoFila - 8.0;
        if (altoBarra > 24.0) altoBarra = 24.0;
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
        fuente.setBold(true);
        p.setFont(fuente);

        for (int i = 0; i < n; i++)
        {
            Taxi *taxi = sim->getTaxi(i);
            double cy = margen + altoFila * (i + 0.5);
            QColor c = taxi->getColor();

            p.setPen(Qt::NoPen);
            p.setBrush(c);
            p.drawEllipse(QPointF(margen + 6, cy), 5.5, 5.5);
            p.setPen(QColor("#dce3ee"));
            p.drawText(QRectF(margen + 18, cy - 11, anchoEtiqueta - 18, 22),
                       Qt::AlignVCenter | Qt::AlignLeft, taxi->getNombre());

            QRectF pista(xPista, cy - altoBarra / 2, anchoPista, altoBarra);
            p.setPen(i == lider && mostrado[i] > 0 ? QPen(QColor("#ffd60a"), 1.5) : Qt::NoPen);
            p.setBrush(QColor("#1a2434"));
            p.drawRoundedRect(pista, 7, 7);

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
                p.drawRoundedRect(relleno, 7, 7);
            }

            p.setPen(i == lider && mostrado[i] > 0 ? QColor("#ffd60a") : QColor("#f4f7fb"));
            p.drawText(QRectF(width() - margen - anchoValor, cy - 11, anchoValor, 22),
                       Qt::AlignVCenter | Qt::AlignRight, Grafo::formatearPesos(mostrado[i]));
        }
    }

public:
    explicit BarrasGananciasWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        sim = nullptr;
        for (int i = 0; i < 7; i++)
        {
            mostrado[i] = 0.0;
        }
        escala = 50000.0;
        setMinimumHeight(250);
    }

    void setSimulacion(Simulacion *nuevaSim)
    {
        sim = nuevaSim;
        connect(sim, &Simulacion::cuadroActualizado, this, &BarrasGananciasWidget::enCuadro);
        connect(sim, &Simulacion::reinicioCompletado, this, &BarrasGananciasWidget::reiniciarBarras);
    }
};

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
    void reconstruirSeries()
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

    void enMuestra(double segundos)
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

public:
    explicit GraficoGananciasWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        sim = nullptr;
        cantidadSeries = 0;
        for (int i = 0; i < 7; i++)
        {
            series[i] = nullptr;
        }

        grafico = new QChart();
        grafico->setTheme(QChart::ChartThemeDark);
        grafico->setBackgroundBrush(QColor("#0c121b"));
        grafico->setTitle(QStringLiteral("Ingresos acumulados por taxi"));
        grafico->setTitleBrush(QColor("#ffd60a"));
        grafico->legend()->setAlignment(Qt::AlignBottom);
        grafico->setMargins(QMargins(4, 4, 4, 4));

        ejeX = new QValueAxis();
        ejeX->setTitleText(QStringLiteral("Tiempo simulado (s)"));
        ejeX->setLabelFormat("%.0f");
        ejeX->setRange(0, 90);

        ejeY = new QValueAxis();
        ejeY->setLabelFormat("%.0f");
        ejeY->setRange(0, 50000);

        grafico->addAxis(ejeX, Qt::AlignBottom);
        grafico->addAxis(ejeY, Qt::AlignLeft);

        vista = new QChartView(grafico, this);
        vista->setRenderHint(QPainter::Antialiasing);
        vista->setBackgroundBrush(QColor("#0c121b"));

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(vista);
    }

    void setSimulacion(Simulacion *nuevaSim)
    {
        sim = nuevaSim;
        connect(sim, &Simulacion::muestraGrafico, this, &GraficoGananciasWidget::enMuestra);
        connect(sim, &Simulacion::reinicioCompletado, this, &GraficoGananciasWidget::reconstruirSeries);
        reconstruirSeries();
    }
};

class DialogoNodo : public QDialog
{
private:
    Ui::NodeDialog *ui;

public:
    explicit DialogoNodo(QWidget *parent = nullptr) : QDialog(parent)
    {
        ui = new Ui::NodeDialog();
        ui->setupUi(this);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Crear"));
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));
    }

    ~DialogoNodo() override
    {
        delete ui;
        ui = nullptr;
    }

    QString getNombreNodo() const
    {
        return ui->edtName->text().trimmed();
    }

    QString getTipoNodo() const
    {
        int idx = ui->comboType->currentIndex();
        if (idx == 1) return "Aeropuerto";
        if (idx == 2) return "Terminal";
        if (idx == 3) return "Turistico";
        return "Normal";
    }
};

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

    void actualizarTextoAyudaEdicion()
    {
        QString texto;
        if (modoEdicion == 0)
            texto = QStringLiteral("Haz clic en un nodo vecino resaltado para mover tu taxi. Clic derecho cancela la ruta.");
        else if (modoEdicion == 1)
            texto = QStringLiteral("Haz clic en un espacio vacío del mapa para agregar un nuevo nodo.");
        else if (modoEdicion == 2)
            texto = QStringLiteral("Haz clic en el nodo origen y luego en el nodo destino para crear la calle.");
        else if (modoEdicion == 3)
            texto = QStringLiteral("Haz clic en los dos extremos de la calle que deseas eliminar.");
        else if (modoEdicion == 4)
            texto = QStringLiteral("Haz clic sobre un nodo para eliminarlo junto con sus calles.");
        else if (modoEdicion == 5)
            texto = QStringLiteral("Haz clic en los dos extremos de la calle para cambiar su ponderación.");
        ui->lblEditHint->setText(texto);
    }

    void mostrarMensajeEstado(QString texto)
    {
        ui->statusbar->showMessage(texto, 8000);
        ui->lblEditHint->setText(texto);
    }

    void configurarPanelGuia()
    {
        ui->txtInfo->setHtml(QStringLiteral(R"(
<h3 style="color:#ffd60a">Resumen de Requerimientos Funcionales</h3>
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

private slots:
    void enPausaCambiada(bool pausada)
    {
        sim->setPausada(pausada);
    }

    void actualizarBotonPausa(bool pausada)
    {
        ui->btnPause->blockSignals(true);
        ui->btnPause->setChecked(pausada);
        ui->btnPause->setText(pausada ? QStringLiteral("Reanudar") : QStringLiteral("Pausar"));
        ui->btnPause->blockSignals(false);
    }

    void enMostrarPesosCambiado(bool activo)
    {
        mapaWidget->setMostrarPesos(activo);
    }

    void enVelocidadCambiada(int indice)
    {
        if (indice == 1) sim->setVelocidad(2.0);
        else if (indice == 2) sim->setVelocidad(4.0);
        else sim->setVelocidad(1.0);
    }

    void enModoTiempoCambiado(int indice)
    {
        sim->setModoTiempo(indice);
    }

    void enModoEdicionCambiado(int indice)
    {
        modoEdicion = indice;
        nodoSeleccionadoEdicion = -1;
        mapaWidget->setNodoPendiente(-1);
        mapaWidget->setModoConducir(modoEdicion == 0);
        actualizarTextoAyudaEdicion();
    }

    void enReiniciarClicado()
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

    void enCancelarClicado()
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

    void enEspacioVacioClicado(QPointF posMapa)
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

    void enNodoClicado(int idNodo)
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

    void agregarEvento(QString texto, QColor color)
    {
        ui->txtLog->append(QStringLiteral("<span style=\"color:%1\">%2</span>")
                               .arg(color.name(), texto.toHtmlEscaped()));
    }

    void configurarTablas()
    {
        QTableWidget *t = ui->tableTaxis;
        t->clear();
        t->setColumnCount(5);
        t->setRowCount(sim->getCantidadTaxis());
        t->setHorizontalHeaderLabels({QStringLiteral("Taxi"), QStringLiteral("Algoritmo"),
                                      QStringLiteral("Viajes"), QStringLiteral("Ingresos"),
                                      QStringLiteral("Explorados")});
        t->verticalHeader()->setVisible(false);
        t->setEditTriggers(QAbstractItemView::NoEditTriggers);
        t->setSelectionMode(QAbstractItemView::NoSelection);
        t->setAlternatingRowColors(true);
        t->setShowGrid(false);
        t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        actualizarTablas();
    }

    void actualizarTablas()
    {
        QTableWidget *t = ui->tableTaxis;
        for (int i = 0; i < sim->getCantidadTaxis(); i++)
        {
            Taxi *tx = sim->getTaxi(i);
            asignarCelda(t, i, 0, QStringLiteral("● ") + tx->getNombre(), tx->getColor(), Qt::AlignLeft);
            asignarCelda(t, i, 1, tx->nombreAlgoritmo(), QColor("#dbe3ef"), Qt::AlignLeft);
            asignarCelda(t, i, 2, QString::number(tx->getViajes()), QColor("#dbe3ef"), Qt::AlignRight);
            asignarCelda(t, i, 3, Grafo::formatearPesos(tx->getIngresos()), QColor("#7dff8a"), Qt::AlignRight);
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
            tp->verticalHeader()->setVisible(false);
            tp->setEditTriggers(QAbstractItemView::NoEditTriggers);
            tp->setSelectionMode(QAbstractItemView::NoSelection);
            tp->setAlternatingRowColors(true);
            tp->setShowGrid(false);
            tp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        }

        const QVector<Pasajero> &lista = sim->getPasajerosEnEspera();
        tp->setRowCount(lista.size());
        Grafo *g = sim->getGrafo();
        int libres = 0;

        for (int i = 0; i < lista.size(); i++)
        {
            const Pasajero &p = lista[i];
            QString estado = QStringLiteral("● LIBRE");
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

        ui->tabs->setTabText(1, QStringLiteral("Pasajeros (%1 libres)").arg(libres));
    }

public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        ui = new Ui::MainWindow();
        sim = new Simulacion(this);
        mapaWidget = nullptr;
        barrasWidget = nullptr;
        graficoWidget = nullptr;
        modoEdicion = 0;
        nodoSeleccionadoEdicion = -1;

        ui->setupUi(this);

        mapaWidget = new MapaWidget(ui->mapHost);
        ui->mapHost->layout()->addWidget(mapaWidget);
        barrasWidget = new BarrasGananciasWidget(ui->barsHost);
        ui->barsHost->layout()->addWidget(barrasWidget);
        graficoWidget = new GraficoGananciasWidget(ui->chartHost);
        ui->chartHost->layout()->addWidget(graficoWidget);

        mapaWidget->setSimulacion(sim);
        barrasWidget->setSimulacion(sim);
        graficoWidget->setSimulacion(sim);

        ui->splitter->setStretchFactor(0, 1);
        ui->splitter->setStretchFactor(1, 0);
        ui->splitter->setSizes({1100, 480});

        configurarTablas();
        configurarPanelGuia();
        actualizarTextoAyudaEdicion();

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
            QStringLiteral("Bienvenido a TaxiGraph. ¡Recoge pasajeros y compara BFS, DFS y Dijkstra!"),
            QColor("#ffd60a"));
        sim->iniciar();
    }

    ~MainWindow() override
    {
        delete ui;
        ui = nullptr;
    }
};

// =====================================================================================
//  FUNCIÓN PRINCIPAL (main)
// =====================================================================================

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("TaxiGraph"));
    app.setStyle(QStringLiteral("Fusion"));
    app.setStyleSheet(QStringLiteral(R"(
* { font-family: "Segoe UI", sans-serif; font-size: 10pt; }
QMainWindow, QDialog, QMessageBox, QInputDialog { background: #0e141d; }
QWidget { color: #dbe3ef; }
QWidget#sidePanel { background: #0e141d; }
QGroupBox {
    background: #141c28; border: 1px solid #223048; border-radius: 10px;
    margin-top: 14px; padding: 10px 8px 8px 8px; font-weight: 600;
}
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; color: #ffd60a; }
QLabel { background: transparent; }
QPushButton {
    background: #1d2a3d; border: 1px solid #2c3d58; border-radius: 8px;
    padding: 7px 12px; font-weight: 600;
}
QPushButton:hover { background: #26374f; border-color: #ffd60a; }
QPushButton:checked { background: #ffd60a; color: #10151e; border-color: #ffd60a; }
QComboBox {
    background: #1b2638; border: 1px solid #2c3d58; border-radius: 8px; padding: 5px 10px;
}
QComboBox:hover { border-color: #ffd60a; }
QComboBox QAbstractItemView {
    background: #1b2638; border: 1px solid #2c3d58; selection-background-color: #ffd60a;
    selection-color: #10151e;
}
QTabWidget::pane { border: 1px solid #223048; border-radius: 8px; background: #101722; top: -1px; }
QTabBar::tab {
    background: #141c28; border: 1px solid #223048; padding: 7px 12px;
    border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 2px; color: #93a4bd;
}
QTabBar::tab:selected { background: #101722; color: #ffd60a; border-bottom-color: #101722; }
QTableWidget {
    background: #101722; alternate-background-color: #141d2b; gridline-color: #1c2739;
    border: none; selection-background-color: #26374f;
}
QHeaderView::section {
    background: #172233; color: #ffd60a; border: none; padding: 5px; font-weight: 600;
}
QTextEdit, QTextBrowser {
    background: #0c121b; border: 1px solid #1c2739; border-radius: 8px; color: #c8d3e4;
}
QSplitter::handle { background: #0b1018; width: 5px; }
QStatusBar { background: #0b1018; color: #93a4bd; }
)"));

    MainWindow ventana;
    ventana.show();
    return app.exec();
}

#include "main.moc"
