#pragma once

class Pasajero
{
private:
    int id;
    int origen;
    int destino;
    int idTaxiAsignado;
    double tarifa;

public:
    Pasajero(int id, int origen, int destino, double tarifa);

    int getId() const;
    int getOrigen() const;
    int getDestino() const;
    int getIdTaxiAsignado() const;
    double getTarifa() const;
    bool estaReservado() const;

    void setIdTaxiAsignado(int idTaxi);
    void setTarifa(double tarifa);
};
