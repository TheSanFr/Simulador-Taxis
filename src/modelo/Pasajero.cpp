#include "modelo/Pasajero.h"

Pasajero::Pasajero(int id, int origen, int destino)
    : id(id), origen(origen), destino(destino)
{
}

int Pasajero::getId() const
{
    return id;
}

int Pasajero::getOrigen() const
{
    return origen;
}

int Pasajero::getDestino() const
{
    return destino;
}

