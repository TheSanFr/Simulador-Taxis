#include "estructuras/NodoCola.h"

NodoCola::NodoCola(int dato)
{
    this->dato = dato;
    this->siguiente = nullptr;
}

int NodoCola::getDato() const
{
    return dato;
}

NodoCola *NodoCola::getSiguiente() const
{
    return siguiente;
}

void NodoCola::setSiguiente(NodoCola *siguiente)
{
    this->siguiente = siguiente;
}
