#pragma once

class Nodo;

class ElementoNodo
{
private:
    Nodo *nodo;
    ElementoNodo *siguiente;

public:
    ElementoNodo(Nodo *nodo);

    Nodo *getNodo() const;
    ElementoNodo *getSiguiente() const;
    void setSiguiente(ElementoNodo *siguiente);
};
