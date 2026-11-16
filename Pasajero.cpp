#include "Pasajero.h"

Pasajero::Pasajero(int id, int origen, int destino, double tarifa)
{
    this->id = id;
    this->origen = origen;
    this->destino = destino;
    this->tarifa = tarifa;
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

double Pasajero::getTarifa() const
{
    return tarifa;
}

void Pasajero::setTarifa(double tarifa)
{
    this->tarifa = tarifa;
}
