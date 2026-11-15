#pragma once

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
