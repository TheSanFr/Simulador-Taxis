#ifndef TAXI_BFS_H
#define TAXI_BFS_H

#include "taxis/Taxi.h"

class TaxiBFS : public Taxi
{
public:
    TaxiBFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    QString nombreAlgoritmo() const override;

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override;
};

#endif
