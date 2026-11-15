#pragma once

#include <QObject>
#include <QPointF>
#include <QString>

#include "ResultadoRuta.h"

class Arista;
class ListaAristas;
class ListaNodos;
class Nodo;

class Grafo : public QObject
{
    Q_OBJECT

private:
    ListaNodos *nodos;
    ListaAristas *aristas;
    double multiplicadorHoraPico;

    ResultadoRuta reconstruirRuta(int anterior[], int origen, int destino, int explorados) const;

public:
    explicit Grafo(QObject *parent = nullptr);

    ~Grafo() override;

    ListaNodos *getNodos() const;
    ListaAristas *getAristas() const;

    Nodo *buscarNodo(int id) const;

    bool tieneNodo(int id) const;

    int obtenerSiguienteIdDisponible() const;

    int agregarNodo(int id, QString nombre, int x, int y, QString tipo);

    int agregarNodoNuevo(QString nombre, QPointF posicion, QString tipo);

    bool eliminarNodo(int id);

    double sugerirPeso(int nodoA, int nodoB, bool esAvenida) const;

    bool agregarArista(int origen, int destino, double peso = -1.0, bool esAvenida = false);

    bool agregarCalleDoble(int nodoA, int nodoB, double peso = -1.0, bool esAvenida = false);

    bool eliminarCalle(int nodoA, int nodoB);

    bool cambiarPeso(int nodoA, int nodoB, double nuevoPeso);

    void limpiar();

    Arista *obtenerArista(int origen, int destino) const;

    bool existeAristaDirigida(int origen, int destino) const;

    bool tieneSalidas(int idNodo) const;

    double obtenerPesoEfectivo(int origen, int destino) const;

    double obtenerPesoEfectivoArista(Arista *a) const;

    double obtenerNivelCongestion(Arista *a) const;

    /** Requerimiento: El peso de cada arista aumenta por uso. */
    void registrarUso(int origen, int destino);

    int nodoMasCercano(QPointF punto, double distanciaMaxima = 1e9) const;

    void setMultiplicadorHoraPico(double multiplicador);

    double getMultiplicadorHoraPico() const;

    // ---- Algoritmo 1: Búsqueda en Anchura (BFS) usando la clase Cola ----
    ResultadoRuta buscarRutaBFS(int origen, int destino) const;

    // ---- Algoritmo 2: Búsqueda en Profundidad (DFS) usando la clase Pila ----
    ResultadoRuta buscarRutaDFS(int origen, int destino) const;

    // ---- Algoritmo 3: Algoritmo de Dijkstra (camino de menor peso ponderado) ----
    ResultadoRuta buscarRutaDijkstra(int origen, int destino) const;

    /**
     * Fórmula del Taxímetro:
     *   Taxímetro = Base ($4,600) + Distancia (salto) x $1,625 + Tiempo (ponderado) x $1,625
     *   Mínimo = $8,100
     *   Recargos: Aeropuerto (+$8,600), Terminal (+$2,400), Turístico (+$2,000), Nocturno (+$2,000)
     */
    static double calcularTarifa(int saltos, double tiempoPonderado, QString tipoDestino, QString periodo);

    static QString formatearPesos(double valor);

signals:
    void grafoModificado();
};
