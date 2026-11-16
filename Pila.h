#ifndef PILA_H
#define PILA_H

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

#endif // PILA_H
