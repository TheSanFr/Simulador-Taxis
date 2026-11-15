#pragma once

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
