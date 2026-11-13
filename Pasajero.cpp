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
    Pasajero(int id, int origen, int destino, double tarifa)
    {
        this->id = id;
        this->origen = origen;
        this->destino = destino;
        this->idTaxiAsignado = -1;
        this->tarifa = tarifa;
    }

    int getId() const { return id; }
    int getOrigen() const { return origen; }
    int getDestino() const { return destino; }
    int getIdTaxiAsignado() const { return idTaxiAsignado; }
    double getTarifa() const { return tarifa; }
    bool estaReservado() const { return idTaxiAsignado >= 0; }

    void setIdTaxiAsignado(int idTaxi) { this->idTaxiAsignado = idTaxi; }
    void setTarifa(double tarifa) { this->tarifa = tarifa; }
};
