#include "Cola.h"

#include "NodoCola.h"

Cola::Cola()
{
    frente = nullptr;
    finalCola = nullptr;
}

Cola::~Cola()
{
    limpiar();
}

void Cola::encolar(int dato)
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

int Cola::desencolar()
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

bool Cola::estaVacia() const
{
    return frente == nullptr;
}

void Cola::limpiar()
{
    while (!estaVacia())
    {
        desencolar();
    }
}
