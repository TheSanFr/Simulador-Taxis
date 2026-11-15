#include "ElementoArista.h"

ElementoArista::ElementoArista(Arista *arista)
{
    this->arista = arista;
    this->siguiente = nullptr;
}

Arista *ElementoArista::getArista() const
{
    return arista;
}

ElementoArista *ElementoArista::getSiguiente() const
{
    return siguiente;
}

void ElementoArista::setSiguiente(ElementoArista *siguiente)
{
    this->siguiente = siguiente;
}
