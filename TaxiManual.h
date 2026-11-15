#pragma once

#include "Taxi.h"

class TaxiManual : public Taxi
{
public:
    TaxiManual(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo);

    QString nombreAlgoritmo() const override;

    bool esManual() const override;

    bool agregarNodoARuta(int idNodo, Grafo *grafo);
};
