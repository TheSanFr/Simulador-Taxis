#pragma once

class NodoPila
{
private:
    int dato;
    NodoPila *siguiente;

public:
    NodoPila(int dato);

    int getDato() const;
    NodoPila *getSiguiente() const;
    void setSiguiente(NodoPila *siguiente);
};
