#pragma once

#include "Taxi.cpp"

class TaxiBFS : public Taxi
{
public:
    TaxiBFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
        : Taxi(id, nombre, color, nodoInicial, grafo)
    {
    }

    QString nombreAlgoritmo() const override
    {
        return QStringLiteral("BFS");
    }

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override
    {
        ResultadoRuta res = grafo->buscarRutaBFS(origen, destino);
        sumarExplorados(res.getNodosExplorados());
        return res;
    }
};
