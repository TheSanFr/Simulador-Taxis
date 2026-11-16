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
