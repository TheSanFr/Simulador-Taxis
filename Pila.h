#pragma once

class NodoPila;

class Pila
{
private:
    NodoPila *cima;

public:
    Pila();

    ~Pila();

    void apilar(int dato);

    int desapilar();

    bool estaVacia() const;

    void limpiar();
};
