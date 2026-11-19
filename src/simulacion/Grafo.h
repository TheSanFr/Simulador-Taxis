#ifndef GRAFO_H
#define GRAFO_H

#include <QObject>
#include <QPair>
#include <QPointF>
#include <QString>
#include <QVector>

#include "modelo/ResultadoRuta.h"

class Arista;
class ListaAristas;
class ListaNodos;
class Nodo;

class Grafo : public QObject
{
    Q_OBJECT

private:
    static const int limiteNodos = 100;
    ListaNodos *nodos;
    ListaAristas *aristas;
    double multiplicadorHoraPico;
    quint64 identidadesNodos[limiteNodos] = {};
    quint64 siguienteIdentidadNodo = 1;
    quint64 revisionPesos = 0;

    ResultadoRuta reconstruirRuta(int anterior[], int origen, int destino, int explorados) const;

public:
    struct RevisionCalle
    {
        bool valida = false;
        QString motivo;
        QVector<QPointF> cruces;
        double peso = 0.0;
        QVector<QPair<int, int>> tramosAfectados;
    };

    struct RevisionNodo
    {
        bool valida = false;
        QString motivo;
        QPointF posicion;
        bool divideCalle = false;
        QVector<QPair<int, int>> tramosAfectados;
    };

    explicit Grafo(QObject *parent = nullptr);

    ~Grafo() override;

    ListaNodos *getNodos() const;
    ListaAristas *getAristas() const;

    Nodo *buscarNodo(int id) const;

    bool tieneNodo(int id) const;
    quint64 getIdentidadNodo(int id) const;
    quint64 getRevisionPesos() const;

    int obtenerSiguienteIdDisponible() const;

    int agregarNodo(int id, QString nombre, int x, int y, QString tipo);

    int agregarNodoNuevo(QString nombre, QPointF posicion, QString tipo);

    bool eliminarNodo(int id);

    double sugerirPeso(int nodoA, int nodoB, bool esAvenida) const;

    bool agregarArista(int origen, int destino, double peso = -1.0, bool esAvenida = false);

    bool agregarCalleDoble(int nodoA, int nodoB, double peso = -1.0, bool esAvenida = false);

    RevisionCalle previsualizarCalle(int origen, int destino, bool dobleSentido) const;
    bool crearCalleEditada(int origen, int destino, bool dobleSentido, bool conectarCruces,
                           QString *motivo = nullptr);
    RevisionNodo previsualizarNodo(QPointF posicion, double radioCalle = 16.0) const;
    int crearNodoEditado(QPointF posicion, QString nombre, QString tipo, bool dividirCalle,
                         QString *motivo = nullptr, double radioCalle = 16.0);

    bool eliminarCalle(int nodoA, int nodoB);

    bool cambiarPeso(int nodoA, int nodoB, double nuevoPeso);

    void limpiar();

    Arista *obtenerArista(int origen, int destino) const;

    bool existeAristaDirigida(int origen, int destino) const;


    double obtenerPesoEfectivo(int origen, int destino) const;

    double obtenerPesoEfectivoArista(Arista *a) const;

    double obtenerNivelCongestion(Arista *a) const;

    void registrarUso(int origen, int destino);

    int nodoMasCercano(QPointF punto, double distanciaMaxima = -1.0) const;

    void setMultiplicadorHoraPico(double multiplicador);

    double getMultiplicadorHoraPico() const;

    ResultadoRuta buscarRutaBFS(int origen, int destino) const;

    ResultadoRuta buscarRutaDFS(int origen, int destino) const;

    ResultadoRuta buscarRutaDijkstra(int origen, int destino) const;

    static double calcularTarifa(int saltos, double tiempoPonderado, QString tipoDestino, QString periodo);

    static QString formatearPesos(double valor);

signals:
    void grafoModificado();
};

#endif
