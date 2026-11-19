#ifndef COLA_H
#define COLA_H

class NodoCola;

class Cola
{
private:
    NodoCola *frente;
    NodoCola *finalCola;

public:
    Cola();

    Cola(const Cola &) = delete;
    Cola &operator=(const Cola &) = delete;

    ~Cola();

    void encolar(int dato);

    int desencolar();

    bool estaVacia() const;

    void limpiar();
};

#endif
