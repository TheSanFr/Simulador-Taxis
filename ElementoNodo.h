#ifndef ELEMENTO_NODO_H
#define ELEMENTO_NODO_H

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

#endif // ELEMENTO_NODO_H
