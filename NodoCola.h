#ifndef NODO_COLA_H
#define NODO_COLA_H

class NodoCola
{
private:
    int dato;
    NodoCola *siguiente;

public:
    NodoCola(int dato);

    int getDato() const;
    NodoCola *getSiguiente() const;
    void setSiguiente(NodoCola *siguiente);
};

#endif // NODO_COLA_H
