#ifndef PASAJERO_H
#define PASAJERO_H

class Pasajero
{
private:
    int id;
    int origen;
    int destino;
    double tarifa;

public:
    Pasajero(int id, int origen, int destino, double tarifa);

    int getId() const;
    int getOrigen() const;
    int getDestino() const;
    double getTarifa() const;

    void setTarifa(double tarifa);
};

#endif // PASAJERO_H
