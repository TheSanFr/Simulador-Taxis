#ifndef PASAJERO_H
#define PASAJERO_H

class Pasajero
{
private:
    int id;
    int origen;
    int destino;

public:
    Pasajero(int id, int origen, int destino);

    int getId() const;
    int getOrigen() const;
    int getDestino() const;
};

#endif
