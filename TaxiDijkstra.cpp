#include "TaxiDijkstra.h"

#include "Grafo.h"

TaxiDijkstra::TaxiDijkstra(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
    : Taxi(id, nombre, color, nodoInicial, grafo)
{
}

QString TaxiDijkstra::nombreAlgoritmo() const
{
    return QStringLiteral("Dijkstra");
}

ResultadoRuta TaxiDijkstra::calcularRuta(Grafo *grafo, int origen, int destino)
{
    ResultadoRuta res = grafo->buscarRutaDijkstra(origen, destino);
    sumarExplorados(res.getNodosExplorados());
    return res;
}
