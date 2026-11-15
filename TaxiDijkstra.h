#pragma once

#include "Taxi.h"

class TaxiDijkstra : public Taxi
{
public:
    TaxiDijkstra(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    QString nombreAlgoritmo() const override;

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override;
};
