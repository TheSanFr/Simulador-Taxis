#pragma once

#include "NodoPila.cpp"

class Pila
{
private:
    NodoPila *cima;

public:
    Pila()
    {
        cima = nullptr;
    }

    ~Pila()
    {
        limpiar();
    }

    void apilar(int dato)
    {
        NodoPila *nuevo = new NodoPila(dato);
        nuevo->setSiguiente(cima);
        cima = nuevo;
    }

    int desapilar()
    {
        if (estaVacia())
        {
            return -1;
        }

        NodoPila *aux = cima;
        int dato = aux->getDato();
        cima = cima->getSiguiente();
        delete aux;
        return dato;
    }

    bool estaVacia() const
    {
        return cima == nullptr;
    }

    void limpiar()
    {
        while (!estaVacia())
        {
            desapilar();
        }
    }
};
