#pragma once

#include "Nodo.cpp"

class ElementoNodo
{
private:
    Nodo *nodo;
    ElementoNodo *siguiente;

public:
    ElementoNodo(Nodo *nodo)
    {
        this->nodo = nodo;
        this->siguiente = nullptr;
    }

    Nodo *getNodo() const { return nodo; }
    ElementoNodo *getSiguiente() const { return siguiente; }
    void setSiguiente(ElementoNodo *siguiente) { this->siguiente = siguiente; }
};
