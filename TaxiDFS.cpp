#pragma once

#include "Taxi.cpp"

class TaxiDFS : public Taxi
{
public:
    TaxiDFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
        : Taxi(id, nombre, color, nodoInicial, grafo)
    {
    }

    QString nombreAlgoritmo() const override
    {
        return QStringLiteral("DFS");
    }

    ResultadoRuta calcularRuta(Grafo *grafo, int origen, int destino) override
    {
        ResultadoRuta res = grafo->buscarRutaDFS(origen, destino);
        sumarExplorados(res.getNodosExplorados());
        return res;
    }
};
