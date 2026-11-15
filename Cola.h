#pragma once

class NodoCola;

class Cola
{
private:
    NodoCola *frente;
    NodoCola *finalCola;

public:
    Cola();

    ~Cola();

    void encolar(int dato);

    int desencolar();

    bool estaVacia() const;

    void limpiar();
};
