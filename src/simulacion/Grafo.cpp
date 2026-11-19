#include "simulacion/Grafo.h"

#include "modelo/Arista.h"
#include "estructuras/Cola.h"
#include "estructuras/ElementoArista.h"
#include "estructuras/ElementoNodo.h"
#include "estructuras/ListaAristas.h"
#include "estructuras/ListaNodos.h"
#include "modelo/Nodo.h"
#include "estructuras/Pila.h"

#include <QLineF>
#include <QLocale>
#include <QSet>
#include <QtMath>
#include <cmath>

const int limiteMaximoNodos = 100;
const double distanciaMinimaCruces = 26.0;
const double toleranciaCruce = 0.75;
const double coordenadaMaximaMapa = 1000000.0;

RevisionCalle::RevisionCalle()
    : valida(false), peso(0.0)
{
}

bool RevisionCalle::esValida() const
{
    return valida;
}

void RevisionCalle::setValida(bool valor)
{
    valida = valor;
}

QString RevisionCalle::getMotivo() const
{
    return motivo;
}

void RevisionCalle::setMotivo(QString texto)
{
    motivo = texto;
}

const QVector<QPointF> &RevisionCalle::getCruces() const
{
    return cruces;
}

void RevisionCalle::agregarCruce(QPointF punto)
{
    cruces.append(punto);
}

double RevisionCalle::getPeso() const
{
    return peso;
}

void RevisionCalle::setPeso(double valor)
{
    peso = valor;
}

const QVector<QPair<int, int>> &RevisionCalle::getTramosAfectados() const
{
    return tramosAfectados;
}

void RevisionCalle::setTramosAfectados(const QVector<QPair<int, int>> &tramos)
{
    tramosAfectados = tramos;
}

RevisionNodo::RevisionNodo()
    : valida(false), divideCalle(false)
{
}

bool RevisionNodo::esValida() const
{
    return valida;
}

void RevisionNodo::setValida(bool valor)
{
    valida = valor;
}

QString RevisionNodo::getMotivo() const
{
    return motivo;
}

void RevisionNodo::setMotivo(QString texto)
{
    motivo = texto;
}

QPointF RevisionNodo::getPosicion() const
{
    return posicion;
}

void RevisionNodo::setPosicion(QPointF punto)
{
    posicion = punto;
}

bool RevisionNodo::getDivideCalle() const
{
    return divideCalle;
}

void RevisionNodo::setDivideCalle(bool valor)
{
    divideCalle = valor;
}

const QVector<QPair<int, int>> &RevisionNodo::getTramosAfectados() const
{
    return tramosAfectados;
}

void RevisionNodo::setTramosAfectados(const QVector<QPair<int, int>> &tramos)
{
    tramosAfectados = tramos;
}

class TramoEdicion
{
private:
    QPair<int, int> nodos;
    QLineF linea;

public:
    TramoEdicion()
    {
    }

    TramoEdicion(QPair<int, int> nodos, QLineF linea)
        : nodos(nodos), linea(linea)
    {
    }

    QPair<int, int> getNodos() const
    {
        return nodos;
    }

    QLineF getLinea() const
    {
        return linea;
    }
};

class CruceEdicion
{
private:
    QPointF posicion;
    int nodo;
    double fraccion;

public:
    CruceEdicion()
        : nodo(-1), fraccion(0.0)
    {
    }

    CruceEdicion(QPointF posicion, int nodo, double fraccion)
        : posicion(posicion), nodo(nodo), fraccion(fraccion)
    {
    }

    QPointF getPosicion() const
    {
        return posicion;
    }

    int getNodo() const
    {
        return nodo;
    }

    void setNodo(int idNodo)
    {
        nodo = idNodo;
    }

    double getFraccion() const
    {
        return fraccion;
    }
};

class DivisionEdicion
{
private:
    QPair<int, int> nodos;
    int indiceCruce;
    double fraccion;

public:
    DivisionEdicion()
        : indiceCruce(0), fraccion(0.0)
    {
    }

    DivisionEdicion(QPair<int, int> nodos, int indiceCruce, double fraccion)
        : nodos(nodos), indiceCruce(indiceCruce), fraccion(fraccion)
    {
    }

    QPair<int, int> getNodos() const
    {
        return nodos;
    }

    int getIndiceCruce() const
    {
        return indiceCruce;
    }

    double getFraccion() const
    {
        return fraccion;
    }
};

class PlanEdicion
{
private:
    QString motivo;
    double peso;
    QVector<CruceEdicion> cruces;
    QVector<DivisionEdicion> divisiones;
    QVector<int> recorrido;

public:
    PlanEdicion()
        : peso(0.0)
    {
    }

    QString getMotivo() const
    {
        return motivo;
    }

    void setMotivo(QString texto)
    {
        motivo = texto;
    }

    double getPeso() const
    {
        return peso;
    }

    void setPeso(double valor)
    {
        peso = valor;
    }

    QVector<CruceEdicion> &getCruces()
    {
        return cruces;
    }

    const QVector<CruceEdicion> &getCruces() const
    {
        return cruces;
    }

    QVector<DivisionEdicion> &getDivisiones()
    {
        return divisiones;
    }

    const QVector<DivisionEdicion> &getDivisiones() const
    {
        return divisiones;
    }

    QVector<int> &getRecorrido()
    {
        return recorrido;
    }

    const QVector<int> &getRecorrido() const
    {
        return recorrido;
    }
};

class AristaPlaneada
{
private:
    int origen;
    int destino;
    double pesoInicial;
    double peso;
    bool esAvenida;

public:
    AristaPlaneada()
        : origen(-1), destino(-1), pesoInicial(0.0), peso(0.0), esAvenida(false)
    {
    }

    AristaPlaneada(int origen, int destino, double pesoInicial, double peso, bool esAvenida)
        : origen(origen), destino(destino), pesoInicial(pesoInicial), peso(peso), esAvenida(esAvenida)
    {
    }

    int getOrigen() const
    {
        return origen;
    }

    int getDestino() const
    {
        return destino;
    }

    double getPesoInicial() const
    {
        return pesoInicial;
    }

    double getPeso() const
    {
        return peso;
    }

    bool getEsAvenida() const
    {
        return esAvenida;
    }
};

double proyectarPuntoEnLinea(const QLineF &linea, QPointF punto)
{
    const QPointF vector = linea.p2() - linea.p1();
    return QPointF::dotProduct(punto - linea.p1(), vector) / QPointF::dotProduct(vector, vector);
}

QPointF redondearCoordenadaNodo(QPointF punto)
{
    return QPointF(qRound(punto.x()), qRound(punto.y()));
}

bool esFraccionInterior(const QLineF &linea, double fraccion)
{
    const double margen = toleranciaCruce / linea.length();
    return fraccion > margen && fraccion < 1.0 - margen;
}

QVector<TramoEdicion> obtenerCallesExistentes(const Grafo &grafo)
{
    QVector<TramoEdicion> calles;
    QSet<QPair<int, int>> vistos;
    for (const ElementoArista *e = grafo.getAristas()->getPrimero(); e != nullptr; e = e->getSiguiente())
    {
        const Arista *arista = e->getArista();
        if (arista == nullptr)
            continue;
        const int a = qMin(arista->getOrigen(), arista->getDestino());
        const int b = qMax(arista->getOrigen(), arista->getDestino());
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
                calles.append(TramoEdicion(ids, linea));
        }
    }
    return calles;
}

bool solapaTramoExistente(const QLineF &propuesta, const QLineF &existente)
{
    const double a = proyectarPuntoEnLinea(propuesta, existente.p1());
    const double b = proyectarPuntoEnLinea(propuesta, existente.p2());
    if (QLineF(propuesta.pointAt(a), existente.p1()).length() > toleranciaCruce
        || QLineF(propuesta.pointAt(b), existente.p2()).length() > toleranciaCruce)
        return false;
    return (qMin(1.0, qMax(a, b)) - qMax(0.0, qMin(a, b))) * propuesta.length()
        > toleranciaCruce;
}

int registrarCruceEnPlan(PlanEdicion &plan, QPointF posicion, int nodo, double fraccion)
{
    QVector<CruceEdicion> &cruces = plan.getCruces();
    for (int i = 0; i < cruces.size(); ++i)
    {
        if (cruces[i].getPosicion() == posicion)
        {
            if (nodo >= 0)
                cruces[i].setNodo(nodo);
            return i;
        }
    }
    cruces.append(CruceEdicion(posicion, nodo, fraccion));
    return cruces.size() - 1;
}

void comprobarCapacidadGrafo(const Grafo &grafo, PlanEdicion &plan)
{
    int nuevos = 0;
    const QVector<CruceEdicion> &cruces = plan.getCruces();
    for (int i = 0; i < cruces.size(); ++i)
    {
        if (cruces[i].getNodo() < 0)
            ++nuevos;
    }
    if (nuevos > limiteMaximoNodos - grafo.getNodos()->getCantidad())
        plan.setMotivo(QStringLiteral("El mapa admite hasta 100 nodos. Libera espacio para crear el cruce."));
}

PlanEdicion prepararPlanCalle(const Grafo &grafo, int origen, int destino)
{
    PlanEdicion plan;
    const Nodo *a = grafo.buscarNodo(origen);
    const Nodo *b = grafo.buscarNodo(destino);
    if (a == nullptr || b == nullptr || origen == destino)
    {
        plan.setMotivo(QStringLiteral("Selecciona dos nodos diferentes."));
        return plan;
    }
    const QLineF propuesta(a->getPosicion(), b->getPosicion());
    if (propuesta.length() < distanciaMinimaCruces)
    {
        plan.setMotivo(QStringLiteral("Los nodos están demasiado cerca para dibujar una calle."));
        return plan;
    }
    plan.setPeso(grafo.sugerirPeso(origen, destino, false));
    registrarCruceEnPlan(plan, propuesta.p1(), origen, 0.0);
    registrarCruceEnPlan(plan, propuesta.p2(), destino, 1.0);

    for (const ElementoNodo *e = grafo.getNodos()->getPrimero(); e != nullptr; e = e->getSiguiente())
    {
        const Nodo *nodo = e->getNodo();
        if (nodo == nullptr || nodo->getId() == origen || nodo->getId() == destino)
            continue;
        const QPointF posicion = nodo->getPosicion();
        const double fraccion = proyectarPuntoEnLinea(propuesta, posicion);
        if (esFraccionInterior(propuesta, fraccion)
            && QLineF(posicion, propuesta.pointAt(fraccion)).length() <= toleranciaCruce)
            registrarCruceEnPlan(plan, posicion, nodo->getId(), fraccion);
    }

    const QVector<TramoEdicion> existentes = obtenerCallesExistentes(grafo);
    for (int i = 0; i < existentes.size(); ++i)
    {
        const TramoEdicion &tramo = existentes[i];
        const QLineF lineaTramo = tramo.getLinea();
        if (solapaTramoExistente(propuesta, lineaTramo))
        {
            plan.setMotivo(QStringLiteral("La propuesta se superpone a una calle existente."));
            return plan;
        }
        QPointF interseccion;
        if (propuesta.intersects(lineaTramo, &interseccion) != QLineF::BoundedIntersection)
            continue;
        const double avance = qBound(0.0, proyectarPuntoEnLinea(propuesta, interseccion), 1.0);
        const double avanceTramo = qBound(0.0, proyectarPuntoEnLinea(lineaTramo, interseccion), 1.0);
        if (!esFraccionInterior(lineaTramo, avanceTramo))
            continue;

        const int idExistente = grafo.nodoMasCercano(interseccion, toleranciaCruce);
        const Nodo *existente = grafo.buscarNodo(idExistente);
        QPointF punto = existente != nullptr ? existente->getPosicion() : redondearCoordenadaNodo(interseccion);
        if (existente == nullptr && grafo.nodoMasCercano(punto, distanciaMinimaCruces) >= 0)
        {
            plan.setMotivo(QStringLiteral("El cruce quedaría demasiado cerca de un nodo existente. Conecta ese nodo."));
            return plan;
        }
        const int indice = registrarCruceEnPlan(plan, punto, idExistente, avance);
        plan.getDivisiones().append(DivisionEdicion(tramo.getNodos(), indice, avanceTramo));
    }

    QVector<int> &recorrido = plan.getRecorrido();
    const QVector<CruceEdicion> &cruces = plan.getCruces();
    for (int i = 0; i < cruces.size(); ++i)
        recorrido.append(i);
    for (int i = 1; i < recorrido.size(); ++i)
    {
        const int indice = recorrido[i];
        int posicion = i;
        while (posicion > 0
               && cruces[recorrido[posicion - 1]].getFraccion() > cruces[indice].getFraccion())
        {
            recorrido[posicion] = recorrido[posicion - 1];
            --posicion;
        }
        recorrido[posicion] = indice;
    }
    for (int i = 1; i < recorrido.size(); ++i)
    {
        const CruceEdicion &anterior = cruces[recorrido[i - 1]];
        const CruceEdicion &actual = cruces[recorrido[i]];
        if (QLineF(anterior.getPosicion(), actual.getPosicion()).length() < distanciaMinimaCruces)
        {
            plan.setMotivo(QStringLiteral("Hay dos cruces demasiado juntos para construir esta calle."));
            return plan;
        }
    }
    comprobarCapacidadGrafo(grafo, plan);
    return plan;
}

PlanEdicion prepararPlanNodo(const Grafo &grafo, QPointF posicion, double radioCalle)
{
    PlanEdicion plan;
    if (!std::isfinite(posicion.x()) || !std::isfinite(posicion.y())
        || std::abs(posicion.x()) > coordenadaMaximaMapa || std::abs(posicion.y()) > coordenadaMaximaMapa
        || !std::isfinite(radioCalle) || radioCalle < 0)
    {
        plan.setMotivo(QStringLiteral("Selecciona una posición válida del mapa."));
        return plan;
    }
    QPointF punto = posicion;
    double menorDistancia = radioCalle;
    const QVector<TramoEdicion> existentes = obtenerCallesExistentes(grafo);
    for (int i = 0; i < existentes.size(); ++i)
    {
        const QLineF lineaTramo = existentes[i].getLinea();
        const double avance = proyectarPuntoEnLinea(lineaTramo, posicion);
        if (!esFraccionInterior(lineaTramo, avance))
            continue;
        const QPointF proyeccion = lineaTramo.pointAt(avance);
        const double distancia = QLineF(proyeccion, posicion).length();
        if (distancia <= menorDistancia)
        {
            punto = proyeccion;
            menorDistancia = distancia;
        }
    }
    registrarCruceEnPlan(plan, redondearCoordenadaNodo(punto), -1, 0.0);
    if (grafo.nodoMasCercano(plan.getCruces().first().getPosicion(), distanciaMinimaCruces) >= 0)
    {
        plan.setMotivo(QStringLiteral("Ya hay un nodo cerca. Usa ese cruce o elige otro punto."));
        return plan;
    }
    for (int i = 0; i < existentes.size(); ++i)
    {
        const TramoEdicion &tramo = existentes[i];
        const QLineF lineaTramo = tramo.getLinea();
        const double avance = proyectarPuntoEnLinea(lineaTramo, punto);
        if (esFraccionInterior(lineaTramo, avance)
            && QLineF(lineaTramo.pointAt(avance), punto).length() <= toleranciaCruce)
            plan.getDivisiones().append(DivisionEdicion(tramo.getNodos(), 0, avance));
    }
    comprobarCapacidadGrafo(grafo, plan);
    return plan;
}

QVector<QPair<int, int>> obtenerTramosAfectados(const PlanEdicion &plan)
{
    QVector<QPair<int, int>> afectados;
    const QVector<DivisionEdicion> &divisiones = plan.getDivisiones();
    for (int i = 0; i < divisiones.size(); ++i)
        afectados.append(divisiones[i].getNodos());
    return afectados;
}

void reservarIdsCruces(const Grafo &grafo, PlanEdicion &plan)
{
    int siguiente = 0;
    QVector<CruceEdicion> &cruces = plan.getCruces();
    for (int i = 0; i < cruces.size(); ++i)
    {
        if (cruces[i].getNodo() >= 0)
            continue;
        while (grafo.tieneNodo(siguiente))
            ++siguiente;
        cruces[i].setNodo(siguiente++);
    }
}

void agregarDireccionDivision(const Grafo &grafo, int desde, int hasta, int cruce,
                              double fraccion, QVector<AristaPlaneada> &altas,
                              QVector<QPair<int, int>> &bajas)
{
    const Arista *arista = grafo.obtenerArista(desde, hasta);
    if (arista == nullptr)
        return;
    const QPair<int, int> par(desde, hasta);
    if (!bajas.contains(par))
        bajas.append(par);
    const double base = arista->getPesoInicial();
    const double actual = arista->getPeso();
    const bool avenida = arista->getEsAvenida();
    altas.append(AristaPlaneada(desde, cruce, base * fraccion, actual * fraccion, avenida));
    altas.append(AristaPlaneada(cruce, hasta, base * (1.0 - fraccion), actual * (1.0 - fraccion), avenida));
}

void planearDivisionTramo(const Grafo &grafo, const DivisionEdicion &division, int cruce,
                          QVector<AristaPlaneada> &altas, QVector<QPair<int, int>> &bajas)
{
    const QPair<int, int> nodos = division.getNodos();
    const double fraccion = division.getFraccion();
    agregarDireccionDivision(grafo, nodos.first, nodos.second, cruce, fraccion, altas, bajas);
    agregarDireccionDivision(grafo, nodos.second, nodos.first, cruce, 1.0 - fraccion, altas, bajas);
}

QString comprobarAristasPlaneadas(const Grafo &grafo, const QVector<AristaPlaneada> &altas,
                                  const QVector<QPair<int, int>> &bajas)
{
    QVector<QPair<int, int>> vistas;
    for (int i = 0; i < altas.size(); ++i)
    {
        const AristaPlaneada &arista = altas[i];
        const QPair<int, int> ids(arista.getOrigen(), arista.getDestino());
        if (ids.first == ids.second || arista.getPeso() <= 0.0 || arista.getPesoInicial() <= 0.0
            || !std::isfinite(arista.getPeso()) || !std::isfinite(arista.getPesoInicial()))
            return QStringLiteral("Hay un tramo demasiado corto para dividir esta calle.");
        if (vistas.contains(ids)
            || (grafo.existeAristaDirigida(ids.first, ids.second) && !bajas.contains(ids)))
            return QStringLiteral("La edición duplicaría un tramo existente.");
        vistas.append(ids);
    }
    return {};
}

void aplicarAristasPlaneadas(Grafo &grafo, const QVector<AristaPlaneada> &altas,
                             const QVector<QPair<int, int>> &bajas)
{
    ListaAristas *aristas = grafo.getAristas();
    for (int i = 0; i < bajas.size(); ++i)
        aristas->eliminarArista(bajas[i].first, bajas[i].second);
    for (int i = 0; i < altas.size(); ++i)
    {
        const AristaPlaneada &datos = altas[i];
        Arista *arista = new Arista(datos.getOrigen(), datos.getDestino(), datos.getPeso(), datos.getEsAvenida());
        arista->setPesoInicial(datos.getPesoInicial());
        aristas->agregar(arista);
    }
}

void Grafo::registrarModificacionEstructura()
{
    ++revisionPesos;
    ++revisionGrafo;
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

    int inverso[limiteMaximoNodos];
    int total = 0;
    int actual = destino;

    while (actual != -1 && total < limiteMaximoNodos)
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

Grafo::Grafo()
{
    nodos = new ListaNodos();
    aristas = new ListaAristas();
    multiplicadorHoraPico = 1.0;
    for (int i = 0; i < limiteMaximoNodos; ++i)
    {
        identidadesNodos[i] = 0;
    }
    siguienteIdentidadNodo = 1;
    revisionPesos = 0;
    revisionGrafo = 0;
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

int Grafo::getIdentidadNodo(int id) const
{
    return id >= 0 && id < limiteMaximoNodos ? identidadesNodos[id] : 0;
}

int Grafo::getRevisionPesos() const
{
    return revisionPesos;
}

int Grafo::getRevisionGrafo() const
{
    return revisionGrafo;
}

int Grafo::obtenerSiguienteIdDisponible() const
{
    for (int i = 0; i < limiteMaximoNodos; i++)
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
    if (id < 0 || id >= limiteMaximoNodos || buscarNodo(id) != nullptr)
    {
        return -1;
    }

    nodos->agregar(new Nodo(id, nombre, x, y, tipo));
    identidadesNodos[id] = siguienteIdentidadNodo++;
    registrarModificacionEstructura();
    return id;
}

int Grafo::agregarNodoNuevo(QString nombre, QPointF posicion, QString tipo)
{
    if (!std::isfinite(posicion.x()) || !std::isfinite(posicion.y())
        || posicion.x() < -coordenadaMaximaMapa
        || posicion.x() > coordenadaMaximaMapa
        || posicion.y() < -coordenadaMaximaMapa
        || posicion.y() > coordenadaMaximaMapa)
        return -1;

    int id = obtenerSiguienteIdDisponible();
    if (id < 0)
    {
        return -1;
    }
    return agregarNodo(id, nombre, int(posicion.x()), int(posicion.y()), tipo);
}

bool Grafo::eliminarNodo(int id)
{
    if (buscarNodo(id) == nullptr)
    {
        return false;
    }

    aristas->eliminarAristasDeNodo(id);
    bool ok = nodos->eliminarPorId(id);
    identidadesNodos[id] = 0;
    registrarModificacionEstructura();
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

    double distancia = QLineF(a->getPosicion(), b->getPosicion()).length() / 400.0;
    if (esAvenida)
    {
        distancia = distancia * 0.75;
    }

    double redondeado = std::round(distancia * 10.0) / 10.0;
    if (redondeado < 0.25)
    {
        redondeado = 0.25;
    }
    return redondeado;
}

bool Grafo::agregarArista(int origen, int destino, double peso, bool esAvenida)
{
    if (!std::isfinite(peso) || origen == destino
        || buscarNodo(origen) == nullptr || buscarNodo(destino) == nullptr)
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
    registrarModificacionEstructura();
    return true;
}

bool Grafo::agregarCalleDoble(int nodoA, int nodoB, double peso, bool esAvenida)
{
    if (!std::isfinite(peso))
        return false;

    double pesoCalculado = peso;
    if (pesoCalculado <= 0.0)
    {
        pesoCalculado = sugerirPeso(nodoA, nodoB, esAvenida);
    }

    bool ab = agregarArista(nodoA, nodoB, pesoCalculado, esAvenida);
    bool ba = agregarArista(nodoB, nodoA, pesoCalculado, esAvenida);
    return ab || ba;
}

RevisionCalle Grafo::previsualizarCalle(int origen, int destino, bool) const
{
    const PlanEdicion plan = prepararPlanCalle(*this, origen, destino);
    RevisionCalle revision;
    revision.setValida(plan.getMotivo().isEmpty());
    revision.setMotivo(plan.getMotivo());
    revision.setPeso(plan.getPeso());
    revision.setTramosAfectados(obtenerTramosAfectados(plan));
    const QVector<int> &recorrido = plan.getRecorrido();
    const QVector<CruceEdicion> &cruces = plan.getCruces();
    const QVector<DivisionEdicion> &divisiones = plan.getDivisiones();
    for (int i = 0; i < recorrido.size(); ++i)
    {
        const int indice = recorrido[i];
        const CruceEdicion &cruce = cruces[indice];
        bool divide = false;
        for (int j = 0; j < divisiones.size(); ++j)
        {
            if (divisiones[j].getIndiceCruce() == indice)
            {
                divide = true;
                break;
            }
        }
        if (divide || (cruce.getFraccion() > 0.0 && cruce.getFraccion() < 1.0))
            revision.agregarCruce(cruce.getPosicion());
    }
    return revision;
}

bool Grafo::crearCalleEditada(int origen, int destino, bool dobleSentido, bool conectarCruces,
                              QString *motivo)
{
    PlanEdicion plan = prepararPlanCalle(*this, origen, destino);
    if (plan.getMotivo().isEmpty() && !conectarCruces
        && (!plan.getDivisiones().isEmpty() || plan.getCruces().size() > 2))
        plan.setMotivo(QStringLiteral("Confirma los cruces para conectar la calle."));
    if (!plan.getMotivo().isEmpty())
    {
        if (motivo != nullptr)
            *motivo = plan.getMotivo();
        return false;
    }

    QVector<int> nuevos;
    for (int i = 0; i < plan.getCruces().size(); ++i)
        if (plan.getCruces()[i].getNodo() < 0)
            nuevos.append(i);
    reservarIdsCruces(*this, plan);

    QVector<AristaPlaneada> altas;
    QVector<QPair<int, int>> bajas;
    const QVector<DivisionEdicion> &divisiones = plan.getDivisiones();
    const QVector<CruceEdicion> &cruces = plan.getCruces();
    for (int i = 0; i < divisiones.size(); ++i)
        planearDivisionTramo(*this, divisiones[i], cruces[divisiones[i].getIndiceCruce()].getNodo(), altas, bajas);
    const QVector<int> &recorrido = plan.getRecorrido();
    for (int i = 1; i < recorrido.size(); ++i)
    {
        const CruceEdicion &a = cruces[recorrido[i - 1]];
        const CruceEdicion &b = cruces[recorrido[i]];
        const double peso = plan.getPeso() * (b.getFraccion() - a.getFraccion());
        altas.append(AristaPlaneada(a.getNodo(), b.getNodo(), peso, peso, false));
        if (dobleSentido)
            altas.append(AristaPlaneada(b.getNodo(), a.getNodo(), peso, peso, false));
    }

    const QString error = comprobarAristasPlaneadas(*this, altas, bajas);
    if (motivo != nullptr)
        *motivo = error;
    if (!error.isEmpty())
        return false;

    for (int i = 0; i < nuevos.size(); ++i)
    {
        const CruceEdicion &cruce = cruces[nuevos[i]];
        agregarNodo(cruce.getNodo(), QStringLiteral("Cruce %1").arg(cruce.getNodo()),
                    qRound(cruce.getPosicion().x()), qRound(cruce.getPosicion().y()), QStringLiteral("Normal"));
    }
    aplicarAristasPlaneadas(*this, altas, bajas);
    registrarModificacionEstructura();
    return true;
}

RevisionNodo Grafo::previsualizarNodo(QPointF posicion, double radioCalle) const
{
    const PlanEdicion plan = prepararPlanNodo(*this, posicion, radioCalle);
    RevisionNodo revision;
    revision.setValida(plan.getMotivo().isEmpty());
    revision.setMotivo(plan.getMotivo());
    revision.setPosicion(plan.getCruces().isEmpty() ? posicion : plan.getCruces().first().getPosicion());
    revision.setDivideCalle(!plan.getDivisiones().isEmpty());
    revision.setTramosAfectados(obtenerTramosAfectados(plan));
    return revision;
}

int Grafo::crearNodoEditado(QPointF posicion, QString nombre, QString tipo, bool dividirCalle,
                            QString *motivo, double radioCalle)
{
    PlanEdicion plan = prepararPlanNodo(*this, posicion, radioCalle);
    if (plan.getMotivo().isEmpty() && !dividirCalle && !plan.getDivisiones().isEmpty())
        plan.setMotivo(QStringLiteral("Confirma la división de la calle para crear el cruce."));
    if (!plan.getMotivo().isEmpty())
    {
        if (motivo != nullptr)
            *motivo = plan.getMotivo();
        return -1;
    }
    reservarIdsCruces(*this, plan);
    const CruceEdicion &cruce = plan.getCruces().first();
    QVector<AristaPlaneada> altas;
    QVector<QPair<int, int>> bajas;
    const QVector<DivisionEdicion> &divisiones = plan.getDivisiones();
    for (int i = 0; i < divisiones.size(); ++i)
        planearDivisionTramo(*this, divisiones[i], cruce.getNodo(), altas, bajas);
    const QString error = comprobarAristasPlaneadas(*this, altas, bajas);
    if (motivo != nullptr)
        *motivo = error;
    if (!error.isEmpty())
        return -1;

    agregarNodo(cruce.getNodo(), nombre, qRound(cruce.getPosicion().x()), qRound(cruce.getPosicion().y()), tipo);
    aplicarAristasPlaneadas(*this, altas, bajas);
    registrarModificacionEstructura();
    return cruce.getNodo();
}

bool Grafo::eliminarCalle(int nodoA, int nodoB)
{
    bool ab = aristas->eliminarArista(nodoA, nodoB);
    bool ba = aristas->eliminarArista(nodoB, nodoA);
    if (ab || ba)
    {
        registrarModificacionEstructura();
        return true;
    }
    return false;
}

bool Grafo::cambiarPeso(int nodoA, int nodoB, double nuevoPeso)
{
    if (!std::isfinite(nuevoPeso) || nuevoPeso <= 0.0)
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

    registrarModificacionEstructura();
    return true;
}

void Grafo::limpiar()
{
    nodos->limpiar();
    aristas->limpiar();
    for (int i = 0; i < limiteMaximoNodos; ++i)
        identidadesNodos[i] = 0;
    registrarModificacionEstructura();
}

Arista *Grafo::obtenerArista(int origen, int destino) const
{
    return aristas->obtenerArista(origen, destino);
}

bool Grafo::existeAristaDirigida(int origen, int destino) const
{
    return aristas->existeArista(origen, destino);
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

    double maximo = ab->getPesoInicial() * 1.5;
    double nuevoPeso = ab->getPeso() + 0.05;
    if (nuevoPeso > maximo)
    {
        nuevoPeso = maximo;
    }
    Arista *ba = aristas->obtenerArista(destino, origen);
    if (ab->getPeso() == nuevoPeso && (ba == nullptr || ba->getPeso() == nuevoPeso))
        return;

    ab->setPeso(nuevoPeso);
    if (ba != nullptr)
    {
        ba->setPeso(nuevoPeso);
    }
    ++revisionPesos;
}

void Grafo::disiparCongestion(double dt)
{
    if (aristas == nullptr || dt <= 0.0)
    {
        return;
    }

    const double tasaDisipacion = dt * 0.008;
    bool cambio = false;

    for (ElementoArista *elemento = aristas->getPrimero(); elemento != nullptr;
         elemento = elemento->getSiguiente())
    {
        Arista *arista = elemento->getArista();
        if (arista != nullptr && arista->getPeso() > arista->getPesoInicial())
        {
            double nuevoPeso = arista->getPeso() - tasaDisipacion;
            if (nuevoPeso < arista->getPesoInicial())
            {
                nuevoPeso = arista->getPesoInicial();
            }
            if (nuevoPeso != arista->getPeso())
            {
                arista->setPeso(nuevoPeso);
                cambio = true;
            }
        }
    }

    if (cambio)
    {
        ++revisionPesos;
    }
}

int Grafo::nodoMasCercano(QPointF punto, double distanciaMaxima) const
{
    if (!std::isfinite(punto.x()) || !std::isfinite(punto.y())
        || std::isnan(distanciaMaxima))
        return -1;

    int mejorId = -1;
    double mejorDistancia = distanciaMaxima;

    ElementoNodo *actual = nodos->getPrimero();
    while (actual != nullptr)
    {
        Nodo *n = actual->getNodo();
        if (n != nullptr)
        {
            double d = QLineF(punto, n->getPosicion()).length();
            if ((distanciaMaxima < 0.0 || d <= distanciaMaxima)
                && (mejorId < 0 || d <= mejorDistancia))
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
    if (!std::isfinite(multiplicador) || multiplicador <= 0.0
        || multiplicadorHoraPico == multiplicador)
        return;
    multiplicadorHoraPico = multiplicador;
    ++revisionPesos;
}

double Grafo::getMultiplicadorHoraPico() const
{
    return multiplicadorHoraPico;
}

ResultadoRuta Grafo::buscarRutaBFS(int origen, int destino) const
{
    const int MAX = limiteMaximoNodos;
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
    const int MAX = limiteMaximoNodos;
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
    const int MAX = limiteMaximoNodos;
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
        distancia[i] = -1.0;
        anterior[i] = -1;
        visitado[i] = false;
    }

    distancia[origen] = 0.0;
    int explorados = 0;
    int cantidad = nodos->getCantidad();

    for (int i = 0; i < cantidad; i++)
    {
        int actual = -1;
        double menorDistancia = 0.0;

        for (int j = 0; j < MAX; j++)
        {
            if (!visitado[j] && buscarNodo(j) != nullptr && distancia[j] >= 0.0
                && (actual < 0 || distancia[j] < menorDistancia))
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
                    if (distancia[vecino] < 0.0 || nuevoCosto < distancia[vecino])
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

double calcularTaximetro(int saltos, double tiempoPonderado)
{
    double base = 4600.0;
    double costoDistancia = saltos * 1625.0;
    double costoTiempo = tiempoPonderado * 1625.0;
    return base + costoDistancia + costoTiempo;
}

double calcularTarifa(int saltos, double tiempoPonderado, QString tipoDestino, QString periodo)
{
    double taximetro = calcularTaximetro(saltos, tiempoPonderado);

    if (taximetro < 8100.0)
    {
        taximetro = 8100.0;
    }

    return taximetro + calcularRecargos(tipoDestino, periodo);
}

double calcularRecargos(QString tipoDestino, QString periodo)
{
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

    return recargoDestino + recargoNocturno;
}

QString formatearPesos(double valor)
{
    if (!std::isfinite(valor))
        return QStringLiteral("$ —");

    const double absoluto = std::abs(valor);
    if (absoluto >= 1000000000.0)
        return QStringLiteral("$ ") + QLocale(QLocale::English).toString(absoluto, 'f', 0);

    return QStringLiteral("$ ") + QLocale(QLocale::English).toString(qRound(absoluto));
}
