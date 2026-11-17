#include "Simulacion.h"

#include "Arista.h"
#include "ElementoArista.h"
#include "ElementoNodo.h"
#include "Grafo.h"
#include "ListaAristas.h"
#include "ListaNodos.h"
#include "Nodo.h"
#include "ResultadoRuta.h"
#include "Taxi.h"
#include "TaxiBFS.h"
#include "TaxiDFS.h"
#include "TaxiDijkstra.h"
#include "TaxiManual.h"

#include <QHash>
#include <QRandomGenerator>
#include <QSet>
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
    agregarManzana(13, 14, 20, 19, "Estadio");
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
    QHash<int, QVector<int>> salidas;
    for (const ElementoArista *elem = grafo->getAristas()->getPrimero();
         elem != nullptr; elem = elem->getSiguiente())
    {
        const Arista *arista = elem->getArista();
        salidas[arista->getOrigen()].append(arista->getDestino());
    }

    QSet<int> ocupados;
    for (const Pasajero &pasajero : pasajeros)
        ocupados.insert(pasajero.getOrigen());

    QVector<int> origenes;
    for (const ElementoNodo *elem = grafo->getNodos()->getPrimero();
         elem != nullptr; elem = elem->getSiguiente())
    {
        const int id = elem->getNodo()->getId();
        if (salidas.contains(id) && !ocupados.contains(id))
            origenes.append(id);
    }
    if (origenes.isEmpty())
        return;

    QRandomGenerator &rng = *QRandomGenerator::global();
    const int origen = origenes[rng.bounded(static_cast<int>(origenes.size()))];

    QVector<int> alcanzables{origen};
    QSet<int> visitados{origen};
    for (qsizetype i = 0; i < alcanzables.size(); ++i)
    {
        const auto aristas = salidas.constFind(alcanzables[i]);
        if (aristas == salidas.constEnd())
            continue;
        for (int siguiente : aristas.value())
        {
            if (!visitados.contains(siguiente))
            {
                visitados.insert(siguiente);
                alcanzables.append(siguiente);
            }
        }
    }

    const auto pesoDestino = [this](int id) {
        return grafo->buscarNodo(id)->getTipo() == QStringLiteral("Turistico") ? 3 : 1;
    };
    int sumaPesos = 0;
    for (qsizetype i = 1; i < alcanzables.size(); ++i)
        sumaPesos += pesoDestino(alcanzables[i]);
    if (sumaPesos == 0)
        return;

    int eleccion = rng.bounded(sumaPesos);
    for (qsizetype i = 1; i < alcanzables.size(); ++i)
    {
        const int destino = alcanzables[i];
        eleccion -= pesoDestino(destino);
        if (eleccion < 0)
        {
            pasajeros.append(Pasajero(siguienteIdPasajero++, origen, destino));
            return;
        }
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

int Simulacion::elegirPasajeroObjetivo(Taxi *taxi) const
{
    int mejorId = -1;
    double mejorCosto = 999999.0;

    for (const Pasajero &pasajero : pasajeros)
    {
        ResultadoRuta r = taxi->calcularRuta(grafo, taxi->getNodoActual(), pasajero.getOrigen());
        if (r.getEncontrada() && r.getTiempoPonderado() < mejorCosto)
        {
            mejorCosto = r.getTiempoPonderado();
            mejorId = pasajero.getId();
        }
    }

    return mejorId;
}

void Simulacion::revisarParadaTaxi(Taxi *taxi)
{
    if (taxi->getLlevaPasajero() && taxi->getNodoActual() == taxi->getDestinoPasajeroABordo())
    {
        int nodoDest = taxi->getDestinoPasajeroABordo();
        double tarifa = taxi->getTarifaPasajeroABordo();
        taxi->bajarPasajero();

        emit carreraCompletada(taxi->getId(), taxi->getPosicion(), tarifa, taxi->getColor());
        registrarEvento(QStringLiteral("Viajes"),
                        taxi->esManual() ? QStringLiteral("Completaste una carrera")
                                         : QStringLiteral("%1 completó una carrera").arg(taxi->getNombre()),
                        QStringLiteral("Llegada a %1 · +%2").arg(nombreNodo(nodoDest), Grafo::formatearPesos(tarifa)),
                        taxi->getId(), nodoDest);
    }

    if (!taxi->getLlevaPasajero())
    {
        for (int i = 0; i < pasajeros.size(); i++)
        {
            const Pasajero &p = pasajeros[i];
            if (p.getOrigen() == taxi->getNodoActual())
            {
                int origen = p.getOrigen();
                int destino = p.getDestino();
                double tarifa = cotizarCarrera(origen, destino);
                pasajeros.removeAt(i);

                taxi->subirPasajero(destino, tarifa);
                if (!taxi->esManual())
                {
                    taxi->limpiarRuta();
                }
                emit pasajeroRecogido(taxi->getId(), origen, destino);
                registrarEvento(QStringLiteral("Viajes"),
                                taxi->esManual() ? QStringLiteral("Recogiste a un pasajero")
                                                 : QStringLiteral("%1 recogió a un pasajero").arg(taxi->getNombre()),
                                QStringLiteral("%1 → %2 · tarifa %3")
                                    .arg(nombreNodo(origen), nombreNodo(destino), Grafo::formatearPesos(tarifa)),
                                taxi->getId(), origen);
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
        taxi->setIdPasajeroObjetivo(elegirPasajeroObjetivo(taxi));
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

    const int objetivo = taxi->getIdPasajeroObjetivo();
    if (objetivo >= 0 && buscarIndicePasajeroPorId(objetivo) < 0)
    {
        taxi->setIdPasajeroObjetivo(-1);
        taxi->limpiarRuta();
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
        registrarEvento(QStringLiteral("Tráfico"), QStringLiteral("Comenzó la hora pico"),
                        QStringLiteral("El peso efectivo de las calles aumenta ×1,8."));
    }
    else if (p == "Nocturno")
    {
        registrarEvento(QStringLiteral("Tráfico"), QStringLiteral("Comenzó el período nocturno"),
                        QStringLiteral("Las nuevas recogidas tienen un recargo de $2.000."));
    }
    else
    {
        registrarEvento(QStringLiteral("Tráfico"), QStringLiteral("Volvió el tránsito normal"),
                        QStringLiteral("Se aplican los pesos habituales de cada calle."));
    }
}

QString Simulacion::nombreNodo(int id) const
{
    Nodo *n = grafo->buscarNodo(id);
    return (n != nullptr) ? n->getNombre() : QStringLiteral("?");
}

void Simulacion::registrarEvento(QString categoria, QString titulo, QString detalle,
                                int idTaxi, int idNodo)
{
    emit eventoOcurrido(reloj.textoHora(), categoria, titulo, detalle, idTaxi, idNodo);
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
    registrarEvento(QStringLiteral("Sesión"), QStringLiteral("Simulación iniciada"), {});
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
    cuentaRegresivaPasajero = 0.0;
    reloj.reiniciar();
    crearTaxis();
    actualizarPeriodo(true);
    for (int i = 0; i < 6; i++)
    {
        generarPasajero();
    }
    emit reinicioCompletado();
    registrarEvento(QStringLiteral("Sesión"), QStringLiteral("Simulación reiniciada"), {});
}

double Simulacion::cotizarCarrera(int origen, int destino) const
{
    ResultadoRuta r = grafo->buscarRutaDijkstra(origen, destino);
    Nodo *nodoDest = grafo->buscarNodo(destino);
    QString tipoDest = (nodoDest != nullptr) ? nodoDest->getTipo() : QStringLiteral("Normal");
    return Grafo::calcularTarifa(r.getSaltos(), r.getTiempoPonderado(), tipoDest, reloj.periodoActual());
}
