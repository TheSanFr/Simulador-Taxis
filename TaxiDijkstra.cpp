#pragma once

#include "Taxi.cpp"

class TaxiDijkstra : public Taxi
{
public:
    TaxiDijkstra(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
        : Taxi(id, nombre, color, nodoInicial, grafo)
    {
    }

    QString nombreAlgoritmo() const override
    {
        return QStringLiteral("Dijkstra");
    }

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override
    {
        ResultadoRuta res = grafo->buscarRutaDijkstra(origen, destino);
        sumarExplorados(res.getNodosExplorados());
        return res;
    }
};
