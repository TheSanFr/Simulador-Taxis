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
