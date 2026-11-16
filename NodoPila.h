#ifndef NODO_PILA_H
#define NODO_PILA_H

class NodoPila
{
private:
    int dato;
    NodoPila *siguiente;

public:
    NodoPila(int dato);

    int getDato() const;
    NodoPila *getSiguiente() const;
    void setSiguiente(NodoPila *siguiente);
};

#endif // NODO_PILA_H
