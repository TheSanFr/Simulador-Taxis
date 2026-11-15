#pragma once

#include "Taxi.h"

class TaxiDFS : public Taxi
{
public:
    TaxiDFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    QString nombreAlgoritmo() const override;

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override;
};
