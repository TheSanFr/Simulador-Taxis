#pragma once

#include "Arista.cpp"

class ElementoArista
{
private:
    Arista *arista;
    ElementoArista *siguiente;

public:
    ElementoArista(Arista *arista)
    {
        this->arista = arista;
        this->siguiente = nullptr;
    }

    Arista *getArista() const { return arista; }
    ElementoArista *getSiguiente() const { return siguiente; }
    void setSiguiente(ElementoArista *siguiente) { this->siguiente = siguiente; }
};
