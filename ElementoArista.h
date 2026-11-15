#pragma once

class Arista;

class ElementoArista
{
private:
    Arista *arista;
    ElementoArista *siguiente;

public:
    ElementoArista(Arista *arista);

    Arista *getArista() const;
    ElementoArista *getSiguiente() const;
    void setSiguiente(ElementoArista *siguiente);
};
