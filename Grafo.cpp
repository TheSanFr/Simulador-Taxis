#pragma once

#include <QLineF>
#include <QLocale>
#include <QObject>
#include <QPointF>
#include <QString>
#include <QtMath>
#include <cmath>

#include "Arista.cpp"
#include "Cola.cpp"
#include "ElementoArista.cpp"
#include "ElementoNodo.cpp"
#include "ListaAristas.cpp"
#include "ListaNodos.cpp"
#include "Nodo.cpp"
#include "Pila.cpp"
#include "ResultadoRuta.cpp"

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

#ifdef COMPILAR_MOCS
#include "Grafo.moc"
#endif
