#include "taxis/TaxiDFS.h"

#include "simulacion/Grafo.h"

TaxiDFS::TaxiDFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
    : Taxi(id, nombre, color, nodoInicial, grafo)
{
}

QString TaxiDFS::nombreAlgoritmo() const
{
    return QStringLiteral("DFS");
}

ResultadoRuta TaxiDFS::calcularRuta(Grafo *grafo, int origen, int destino)
{
    if (grafo == nullptr)
    {
        return ResultadoRuta();
    }
    ResultadoRuta res = grafo->buscarRutaDFS(origen, destino);
    sumarExplorados(res.getNodosExplorados());
    return res;
}
