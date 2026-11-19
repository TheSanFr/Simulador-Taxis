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
