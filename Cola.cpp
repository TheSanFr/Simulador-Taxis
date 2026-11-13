#pragma once

#include "NodoCola.cpp"

class Cola
{
private:
    NodoCola *frente;
    NodoCola *finalCola;

public:
    Cola()
    {
        frente = nullptr;
        finalCola = nullptr;
    }

    ~Cola()
    {
        limpiar();
    }

    void encolar(int dato)
    {
        NodoCola *nuevo = new NodoCola(dato);

        if (estaVacia())
        {
            frente = nuevo;
            finalCola = nuevo;
        }
        else
        {
            finalCola->setSiguiente(nuevo);
            finalCola = nuevo;
        }
    }

    int desencolar()
    {
        if (estaVacia())
        {
            return -1;
        }

        NodoCola *aux = frente;
        int dato = aux->getDato();

        frente = frente->getSiguiente();
        if (frente == nullptr)
        {
            finalCola = nullptr;
        }

        delete aux;
        return dato;
    }

    bool estaVacia() const
    {
        return frente == nullptr;
    }

    void limpiar()
    {
        while (!estaVacia())
        {
            desencolar();
        }
    }
};
