#pragma once

class NodoCola
{
private:
    int dato;
    NodoCola *siguiente;

public:
    NodoCola(int dato)
    {
        this->dato = dato;
        this->siguiente = nullptr;
    }

    int getDato() const { return dato; }
    NodoCola *getSiguiente() const { return siguiente; }
    void setSiguiente(NodoCola *siguiente) { this->siguiente = siguiente; }
};
