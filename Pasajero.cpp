#include "Pasajero.h"

Pasajero::Pasajero(int id, int origen, int destino, double tarifa)
{
    this->id = id;
    this->origen = origen;
    this->destino = destino;
    this->idTaxiAsignado = -1;
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

int Pasajero::getIdTaxiAsignado() const
{
    return idTaxiAsignado;
}

double Pasajero::getTarifa() const
{
    return tarifa;
}

bool Pasajero::estaReservado() const
{
    return idTaxiAsignado >= 0;
}

void Pasajero::setIdTaxiAsignado(int idTaxi)
{
    this->idTaxiAsignado = idTaxi;
}

void Pasajero::setTarifa(double tarifa)
{
    this->tarifa = tarifa;
}
