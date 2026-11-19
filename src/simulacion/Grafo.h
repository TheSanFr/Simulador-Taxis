#ifndef GRAFO_H
#define GRAFO_H

#include <QPair>
#include <QPointF>
#include <QString>
#include <QVector>

#include "modelo/ResultadoRuta.h"

class Arista;
class ListaAristas;
class ListaNodos;
class Nodo;

double calcularTarifa(int saltos, double tiempoPonderado, QString tipoDestino, QString periodo);
double calcularTaximetro(int saltos, double tiempoPonderado);
double calcularRecargos(QString tipoDestino, QString periodo);
QString formatearPesos(double valor);

class RevisionCalle
{
private:
    bool valida;
    QString motivo;
    QVector<QPointF> cruces;
    double peso;
    QVector<QPair<int, int>> tramosAfectados;

public:
    RevisionCalle();

    bool esValida() const;
    void setValida(bool valor);

    QString getMotivo() const;
    void setMotivo(QString texto);

    const QVector<QPointF> &getCruces() const;
    void agregarCruce(QPointF punto);

    double getPeso() const;
    void setPeso(double valor);

    const QVector<QPair<int, int>> &getTramosAfectados() const;
    void setTramosAfectados(const QVector<QPair<int, int>> &tramos);
};

class RevisionNodo
{
private:
    bool valida;
    QString motivo;
    QPointF posicion;
    bool divideCalle;
    QVector<QPair<int, int>> tramosAfectados;

public:
    RevisionNodo();

    bool esValida() const;
    void setValida(bool valor);

    QString getMotivo() const;
    void setMotivo(QString texto);

    QPointF getPosicion() const;
    void setPosicion(QPointF punto);

    bool getDivideCalle() const;
    void setDivideCalle(bool valor);

    const QVector<QPair<int, int>> &getTramosAfectados() const;
    void setTramosAfectados(const QVector<QPair<int, int>> &tramos);
};

class Grafo
{
private:
    ListaNodos *nodos;
    ListaAristas *aristas;
    double multiplicadorHoraPico;
    int identidadesNodos[100];
    int siguienteIdentidadNodo;
    int revisionPesos;
    int revisionGrafo;

    void registrarModificacionEstructura();
    ResultadoRuta reconstruirRuta(int anterior[], int origen, int destino, int explorados) const;

public:
    Grafo();
    ~Grafo();

    ListaNodos *getNodos() const;
    ListaAristas *getAristas() const;

    Nodo *buscarNodo(int id) const;

    bool tieneNodo(int id) const;
    int getIdentidadNodo(int id) const;
    int getRevisionPesos() const;
    int getRevisionGrafo() const;

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
    void disiparCongestion(double dt);

    int nodoMasCercano(QPointF punto, double distanciaMaxima = -1.0) const;

    void setMultiplicadorHoraPico(double multiplicador);

    double getMultiplicadorHoraPico() const;

    ResultadoRuta buscarRutaBFS(int origen, int destino) const;

    ResultadoRuta buscarRutaDFS(int origen, int destino) const;

    ResultadoRuta buscarRutaDijkstra(int origen, int destino) const;
};

#endif
