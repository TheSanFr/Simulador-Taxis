#include "Pila.h"

#include "NodoPila.h"

Pila::Pila()
{
    cima = nullptr;
}

Pila::~Pila()
{
    limpiar();
}

void Pila::apilar(int dato)
{
    NodoPila *nuevo = new NodoPila(dato);
    nuevo->setSiguiente(cima);
    cima = nuevo;
}

int Pila::desapilar()
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

bool Pila::estaVacia() const
{
    return cima == nullptr;
}

void Pila::limpiar()
{
    while (!estaVacia())
    {
        desapilar();
    }
}
