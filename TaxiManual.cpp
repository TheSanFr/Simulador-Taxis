#include "TaxiManual.h"

#include "Grafo.h"

TaxiManual::TaxiManual(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
    : Taxi(id, nombre, color, nodoInicial, grafo)
{
}

QString TaxiManual::nombreAlgoritmo() const
{
    return QStringLiteral("Manual");
}

bool TaxiManual::esManual() const
{
    return true;
}

bool TaxiManual::agregarNodoARuta(int idNodo, Grafo *grafo)
{
    int desde = nodoFinalDeRuta();
    if (grafo == nullptr || !grafo->existeAristaDirigida(desde, idNodo))
    {
        return false;
    }
    agregarPasoARuta(idNodo);
    return true;
}
