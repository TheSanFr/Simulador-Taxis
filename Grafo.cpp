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
#include <QSet>
#include <QSignalBlocker>
#include <QtMath>
#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
constexpr double distanciaMinimaCruces = 26.0;
constexpr double toleranciaCruce = 0.75;

struct Tramo
{
    QPair<int, int> nodos;
    QLineF linea;
};

struct Cruce
{
    QPointF posicion;
    int nodo;
    double fraccion;
};

struct Division
{
    QPair<int, int> nodos;
    int indiceCruce;
    double fraccion;
};

struct PlanEdicion
{
    QString motivo;
    double peso = 0.0;
    QVector<Cruce> cruces;
    QVector<Division> divisiones;
    QVector<int> recorrido;
};

struct AristaPlaneada
{
    int origen;
    int destino;
    double pesoInicial;
    double peso;
    bool esAvenida;
};

double proyectar(const QLineF &linea, QPointF punto)
{
    const QPointF vector = linea.p2() - linea.p1();
    return QPointF::dotProduct(punto - linea.p1(), vector) / QPointF::dotProduct(vector, vector);
}

QPointF coordenadaNodo(QPointF punto)
{
    return QPointF(qRound(punto.x()), qRound(punto.y()));
}

bool esInterior(const QLineF &linea, double fraccion)
{
    const double margen = toleranciaCruce / linea.length();
    return fraccion > margen && fraccion < 1.0 - margen;
}

QVector<Tramo> callesExistentes(const Grafo &grafo)
{
    QVector<Tramo> calles;
    QSet<QPair<int, int>> vistos;
    for (const ElementoArista *e = grafo.getAristas()->getPrimero(); e != nullptr; e = e->getSiguiente())
    {
        const Arista *arista = e->getArista();
        if (arista == nullptr)
            continue;
        const int a = std::min(arista->getOrigen(), arista->getDestino());
        const int b = std::max(arista->getOrigen(), arista->getDestino());
        const QPair<int, int> ids(a, b);
        if (vistos.contains(ids))
            continue;
        vistos.insert(ids);
        const Nodo *nodoA = grafo.buscarNodo(a);
        const Nodo *nodoB = grafo.buscarNodo(b);
        if (nodoA != nullptr && nodoB != nullptr)
        {
            const QLineF linea(nodoA->getPosicion(), nodoB->getPosicion());
            if (linea.length() > toleranciaCruce)
                calles.append({ids, linea});
        }
    }
    return calles;
}

bool solapaTramo(const QLineF &propuesta, const QLineF &existente)
{
    const double a = proyectar(propuesta, existente.p1());
    const double b = proyectar(propuesta, existente.p2());
    if (QLineF(propuesta.pointAt(a), existente.p1()).length() > toleranciaCruce
        || QLineF(propuesta.pointAt(b), existente.p2()).length() > toleranciaCruce)
        return false;
    return (std::min(1.0, std::max(a, b)) - std::max(0.0, std::min(a, b))) * propuesta.length()
        > toleranciaCruce;
}

int registrarCruce(PlanEdicion &plan, QPointF posicion, int nodo, double fraccion)
{
    for (int i = 0; i < plan.cruces.size(); ++i)
    {
        if (plan.cruces[i].posicion == posicion)
        {
            if (nodo >= 0)
                plan.cruces[i].nodo = nodo;
            return i;
        }
    }
    plan.cruces.append({posicion, nodo, fraccion});
    return plan.cruces.size() - 1;
}

void comprobarCapacidad(const Grafo &grafo, PlanEdicion &plan)
{
    const auto nuevos = std::count_if(plan.cruces.cbegin(), plan.cruces.cend(),
                                     [](const Cruce &cruce) { return cruce.nodo < 0; });
    if (nuevos > 100 - grafo.getNodos()->getCantidad())
        plan.motivo = QStringLiteral("El mapa admite hasta 100 nodos. Libera espacio para crear el cruce.");
}

PlanEdicion prepararCalle(const Grafo &grafo, int origen, int destino)
{
    PlanEdicion plan;
    const Nodo *a = grafo.buscarNodo(origen);
    const Nodo *b = grafo.buscarNodo(destino);
    if (a == nullptr || b == nullptr || origen == destino)
    {
        plan.motivo = QStringLiteral("Selecciona dos nodos diferentes.");
        return plan;
    }
    const QLineF propuesta(a->getPosicion(), b->getPosicion());
    if (propuesta.length() < distanciaMinimaCruces)
    {
        plan.motivo = QStringLiteral("Los nodos están demasiado cerca para dibujar una calle.");
        return plan;
    }
    plan.peso = grafo.sugerirPeso(origen, destino, false);
    registrarCruce(plan, propuesta.p1(), origen, 0.0);
    registrarCruce(plan, propuesta.p2(), destino, 1.0);

    for (const ElementoNodo *e = grafo.getNodos()->getPrimero(); e != nullptr; e = e->getSiguiente())
    {
        const Nodo *nodo = e->getNodo();
        if (nodo == nullptr || nodo->getId() == origen || nodo->getId() == destino)
            continue;
        const QPointF posicion = nodo->getPosicion();
        const double fraccion = proyectar(propuesta, posicion);
        if (esInterior(propuesta, fraccion)
            && QLineF(posicion, propuesta.pointAt(fraccion)).length() <= toleranciaCruce)
            registrarCruce(plan, posicion, nodo->getId(), fraccion);
    }

    for (const Tramo &tramo : callesExistentes(grafo))
    {
        if (solapaTramo(propuesta, tramo.linea))
        {
            plan.motivo = QStringLiteral("La propuesta se superpone a una calle existente.");
            return plan;
        }
        QPointF interseccion;
        if (propuesta.intersects(tramo.linea, &interseccion) != QLineF::BoundedIntersection)
            continue;
        const double avance = std::clamp(proyectar(propuesta, interseccion), 0.0, 1.0);
        const double avanceTramo = std::clamp(proyectar(tramo.linea, interseccion), 0.0, 1.0);
        if (!esInterior(tramo.linea, avanceTramo))
            continue;

        const int idExistente = grafo.nodoMasCercano(interseccion, toleranciaCruce);
        const Nodo *existente = grafo.buscarNodo(idExistente);
        QPointF punto = existente != nullptr ? existente->getPosicion() : coordenadaNodo(interseccion);
        if (existente == nullptr && grafo.nodoMasCercano(punto, distanciaMinimaCruces) >= 0)
        {
            plan.motivo = QStringLiteral("El cruce quedaría demasiado cerca de un nodo existente. Conecta ese nodo.");
            return plan;
        }
        const int indice = registrarCruce(plan, punto, idExistente, avance);
        plan.divisiones.append({tramo.nodos, indice, avanceTramo});
    }

    for (int i = 0; i < plan.cruces.size(); ++i)
        plan.recorrido.append(i);
    std::sort(plan.recorrido.begin(), plan.recorrido.end(),
              [&plan](int a, int b) { return plan.cruces[a].fraccion < plan.cruces[b].fraccion; });
    for (int i = 1; i < plan.recorrido.size(); ++i)
    {
        const Cruce &anterior = plan.cruces[plan.recorrido[i - 1]];
        const Cruce &actual = plan.cruces[plan.recorrido[i]];
        if (QLineF(anterior.posicion, actual.posicion).length() < distanciaMinimaCruces)
        {
            plan.motivo = QStringLiteral("Hay dos cruces demasiado juntos para construir esta calle.");
            return plan;
        }
    }
    comprobarCapacidad(grafo, plan);
    return plan;
}

PlanEdicion prepararNodo(const Grafo &grafo, QPointF posicion, double radioCalle)
{
    PlanEdicion plan;
    const double maximo = std::numeric_limits<int>::max() - 1.0;
    if (!std::isfinite(posicion.x()) || !std::isfinite(posicion.y())
        || std::abs(posicion.x()) > maximo || std::abs(posicion.y()) > maximo
        || !std::isfinite(radioCalle) || radioCalle < 0)
    {
        plan.motivo = QStringLiteral("Selecciona una posición válida del mapa.");
        return plan;
    }
    QPointF punto = posicion;
    double menorDistancia = radioCalle;
    for (const Tramo &tramo : callesExistentes(grafo))
    {
        const double avance = proyectar(tramo.linea, posicion);
        if (!esInterior(tramo.linea, avance))
            continue;
        const QPointF proyeccion = tramo.linea.pointAt(avance);
        const double distancia = QLineF(proyeccion, posicion).length();
        if (distancia <= menorDistancia)
        {
            punto = proyeccion;
            menorDistancia = distancia;
        }
    }
    registrarCruce(plan, coordenadaNodo(punto), -1, 0.0);
    if (grafo.nodoMasCercano(plan.cruces.first().posicion, distanciaMinimaCruces) >= 0)
    {
        plan.motivo = QStringLiteral("Ya hay un nodo cerca. Usa ese cruce o elige otro punto.");
        return plan;
    }
    for (const Tramo &tramo : callesExistentes(grafo))
    {
        const double avance = proyectar(tramo.linea, punto);
        if (esInterior(tramo.linea, avance)
            && QLineF(tramo.linea.pointAt(avance), punto).length() <= toleranciaCruce)
            plan.divisiones.append({tramo.nodos, 0, avance});
    }
    comprobarCapacidad(grafo, plan);
    return plan;
}

QVector<QPair<int, int>> tramosAfectados(const PlanEdicion &plan)
{
    QVector<QPair<int, int>> afectados;
    for (const Division &division : plan.divisiones)
        afectados.append(division.nodos);
    return afectados;
}

void reservarIds(const Grafo &grafo, PlanEdicion &plan)
{
    int siguiente = 0;
    for (Cruce &cruce : plan.cruces)
    {
        if (cruce.nodo >= 0)
            continue;
        while (grafo.tieneNodo(siguiente))
            ++siguiente;
        cruce.nodo = siguiente++;
    }
}

void planearDivision(const Grafo &grafo, const Division &division, int cruce,
                     QVector<AristaPlaneada> &altas, QSet<QPair<int, int>> &bajas)
{
    const auto agregarDireccion = [&](int desde, int hasta, double fraccion) {
        const Arista *arista = grafo.obtenerArista(desde, hasta);
        if (arista == nullptr)
            return;
        bajas.insert({desde, hasta});
        const double base = arista->getPesoInicial();
        const double actual = arista->getPeso();
        const bool avenida = arista->getEsAvenida();
        altas.append({desde, cruce, base * fraccion, actual * fraccion, avenida});
        altas.append({cruce, hasta, base * (1.0 - fraccion), actual * (1.0 - fraccion), avenida});
    };
    agregarDireccion(division.nodos.first, division.nodos.second, division.fraccion);
    agregarDireccion(division.nodos.second, division.nodos.first, 1.0 - division.fraccion);
}

QString comprobarAristas(const Grafo &grafo, const QVector<AristaPlaneada> &altas,
                         const QSet<QPair<int, int>> &bajas)
{
    QSet<QPair<int, int>> vistas;
    for (const AristaPlaneada &arista : altas)
    {
        const QPair<int, int> ids(arista.origen, arista.destino);
        if (ids.first == ids.second || arista.peso <= 0.0 || arista.pesoInicial <= 0.0
            || !std::isfinite(arista.peso) || !std::isfinite(arista.pesoInicial))
            return QStringLiteral("Hay un tramo demasiado corto para dividir esta calle.");
        if (vistas.contains(ids)
            || (grafo.existeAristaDirigida(ids.first, ids.second) && !bajas.contains(ids)))
            return QStringLiteral("La edición duplicaría un tramo existente.");
        vistas.insert(ids);
    }
    return {};
}

void aplicarAristas(Grafo &grafo, const QVector<AristaPlaneada> &altas,
                    const QSet<QPair<int, int>> &bajas)
{
    ListaAristas *aristas = grafo.getAristas();
    for (const auto &ids : bajas)
        aristas->eliminarArista(ids.first, ids.second);
    for (const AristaPlaneada &datos : altas)
    {
        auto *arista = new Arista(datos.origen, datos.destino, datos.peso, datos.esAvenida);
        arista->setPesoInicial(datos.pesoInicial);
        aristas->agregar(arista);
    }
}
}

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

Grafo::RevisionCalle Grafo::previsualizarCalle(int origen, int destino, bool /*dobleSentido*/) const
{
    const PlanEdicion plan = prepararCalle(*this, origen, destino);
    RevisionCalle revision;
    revision.valida = plan.motivo.isEmpty();
    revision.motivo = plan.motivo;
    revision.peso = plan.peso;
    revision.tramosAfectados = tramosAfectados(plan);
    for (int indice : plan.recorrido)
    {
        const Cruce &cruce = plan.cruces[indice];
        const bool divide = std::any_of(plan.divisiones.cbegin(), plan.divisiones.cend(),
                                       [indice](const Division &d) { return d.indiceCruce == indice; });
        if (divide || (cruce.fraccion > 0.0 && cruce.fraccion < 1.0))
            revision.cruces.append(cruce.posicion);
    }
    return revision;
}

bool Grafo::crearCalleEditada(int origen, int destino, bool dobleSentido, bool conectarCruces,
                              QString *motivo)
{
    PlanEdicion plan = prepararCalle(*this, origen, destino);
    if (plan.motivo.isEmpty() && !conectarCruces
        && (!plan.divisiones.isEmpty() || plan.cruces.size() > 2))
        plan.motivo = QStringLiteral("Confirma los cruces para conectar la calle.");
    if (!plan.motivo.isEmpty())
    {
        if (motivo != nullptr)
            *motivo = plan.motivo;
        return false;
    }

    QVector<int> nuevos;
    for (int i = 0; i < plan.cruces.size(); ++i)
        if (plan.cruces[i].nodo < 0)
            nuevos.append(i);
    reservarIds(*this, plan);

    QVector<AristaPlaneada> altas;
    QSet<QPair<int, int>> bajas;
    for (const Division &division : plan.divisiones)
        planearDivision(*this, division, plan.cruces[division.indiceCruce].nodo, altas, bajas);
    for (int i = 1; i < plan.recorrido.size(); ++i)
    {
        const Cruce &a = plan.cruces[plan.recorrido[i - 1]];
        const Cruce &b = plan.cruces[plan.recorrido[i]];
        const double peso = plan.peso * (b.fraccion - a.fraccion);
        altas.append({a.nodo, b.nodo, peso, peso, false});
        if (dobleSentido)
            altas.append({b.nodo, a.nodo, peso, peso, false});
    }

    const QString error = comprobarAristas(*this, altas, bajas);
    if (motivo != nullptr)
        *motivo = error;
    if (!error.isEmpty())
        return false;

    // La interfaz y la simulación ven únicamente el grafo resultante.
    {
        const QSignalBlocker bloquear(this);
        for (int indice : nuevos)
        {
            const Cruce &cruce = plan.cruces[indice];
            agregarNodo(cruce.nodo, QStringLiteral("Cruce %1").arg(cruce.nodo),
                        qRound(cruce.posicion.x()), qRound(cruce.posicion.y()), QStringLiteral("Normal"));
        }
        aplicarAristas(*this, altas, bajas);
    }
    emit grafoModificado();
    return true;
}

Grafo::RevisionNodo Grafo::previsualizarNodo(QPointF posicion, double radioCalle) const
{
    const PlanEdicion plan = prepararNodo(*this, posicion, radioCalle);
    RevisionNodo revision;
    revision.valida = plan.motivo.isEmpty();
    revision.motivo = plan.motivo;
    revision.posicion = plan.cruces.isEmpty() ? posicion : plan.cruces.first().posicion;
    revision.divideCalle = !plan.divisiones.isEmpty();
    revision.tramosAfectados = tramosAfectados(plan);
    return revision;
}

int Grafo::crearNodoEditado(QPointF posicion, QString nombre, QString tipo, bool dividirCalle,
                            QString *motivo, double radioCalle)
{
    PlanEdicion plan = prepararNodo(*this, posicion, radioCalle);
    if (plan.motivo.isEmpty() && !dividirCalle && !plan.divisiones.isEmpty())
        plan.motivo = QStringLiteral("Confirma la división de la calle para crear el cruce.");
    if (!plan.motivo.isEmpty())
    {
        if (motivo != nullptr)
            *motivo = plan.motivo;
        return -1;
    }
    reservarIds(*this, plan);
    const Cruce &cruce = plan.cruces.first();
    QVector<AristaPlaneada> altas;
    QSet<QPair<int, int>> bajas;
    for (const Division &division : plan.divisiones)
        planearDivision(*this, division, cruce.nodo, altas, bajas);
    const QString error = comprobarAristas(*this, altas, bajas);
    if (motivo != nullptr)
        *motivo = error;
    if (!error.isEmpty())
        return -1;

    {
        const QSignalBlocker bloquear(this);
        agregarNodo(cruce.nodo, nombre, qRound(cruce.posicion.x()), qRound(cruce.posicion.y()), tipo);
        aplicarAristas(*this, altas, bajas);
    }
    emit grafoModificado();
    return cruce.nodo;
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
