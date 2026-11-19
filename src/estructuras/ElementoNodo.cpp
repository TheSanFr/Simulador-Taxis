#include "estructuras/ElementoNodo.h"

ElementoNodo::ElementoNodo(Nodo *nodo)
{
    this->nodo = nodo;
    this->siguiente = nullptr;
}

Nodo *ElementoNodo::getNodo() const
{
    return nodo;
}

ElementoNodo *ElementoNodo::getSiguiente() const
{
    return siguiente;
}

void ElementoNodo::setSiguiente(ElementoNodo *siguiente)
{
    this->siguiente = siguiente;
}
