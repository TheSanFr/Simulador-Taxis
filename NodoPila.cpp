#include "NodoPila.h"

NodoPila::NodoPila(int dato)
{
    this->dato = dato;
    this->siguiente = nullptr;
}

int NodoPila::getDato() const
{
    return dato;
}

NodoPila *NodoPila::getSiguiente() const
{
    return siguiente;
}

void NodoPila::setSiguiente(NodoPila *siguiente)
{
    this->siguiente = siguiente;
}
