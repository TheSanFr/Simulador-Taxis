#include "taxis/TaxiBFS.h"

#include "simulacion/Grafo.h"

TaxiBFS::TaxiBFS(int id, QString nombre, QColor color, int nodoInicial, Grafo *grafo)
    : Taxi(id, nombre, color, nodoInicial, grafo)
{
}

QString TaxiBFS::nombreAlgoritmo() const
{
    return QStringLiteral("BFS");
}

ResultadoRuta TaxiBFS::calcularRuta(Grafo *grafo, int origen, int destino)
{
    if (grafo == nullptr)
    {
        return ResultadoRuta();
    }
    ResultadoRuta res = grafo->buscarRutaBFS(origen, destino);
    sumarExplorados(res.getNodosExplorados());
    return res;
}
