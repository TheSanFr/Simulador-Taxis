#pragma once

class NodoPila
{
private:
    int dato;
    NodoPila *siguiente;

public:
    NodoPila(int dato)
    {
        this->dato = dato;
        this->siguiente = nullptr;
    }

    int getDato() const { return dato; }
    NodoPila *getSiguiente() const { return siguiente; }
    void setSiguiente(NodoPila *siguiente) { this->siguiente = siguiente; }
};
