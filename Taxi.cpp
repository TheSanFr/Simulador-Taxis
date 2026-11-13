#pragma once

#include <QColor>
#include <QPointF>
#include <QString>
#include <QtMath>
#include <cmath>

#include "Grafo.cpp"
#include "Nodo.cpp"
#include "ResultadoRuta.cpp"

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
