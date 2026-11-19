#ifndef TAXI_DFS_H
#define TAXI_DFS_H

#include "Taxi.h"

class TaxiDFS : public Taxi
{
public:
    TaxiDFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    QString nombreAlgoritmo() const override;

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override;
};

#endif
