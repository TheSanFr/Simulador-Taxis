#pragma once

#include "Taxi.cpp"

class TaxiManual : public Taxi
{
public:
    TaxiManual(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
        : Taxi(id, nombre, color, nodoInicial, grafo)
    {
    }

    QString nombreAlgoritmo() const override
    {
        return QStringLiteral("Manual");
    }

    bool esManual() const override
    {
        return true;
    }

    bool agregarNodoARuta(int idNodo, Grafo *grafo)
    {
        int desde = nodoFinalDeRuta();
        if (grafo == nullptr || !grafo->existeAristaDirigida(desde, idNodo))
        {
            return false;
        }
        agregarPasoARuta(idNodo);
        return true;
    }
};
