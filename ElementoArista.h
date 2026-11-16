#ifndef ELEMENTO_ARISTA_H
#define ELEMENTO_ARISTA_H

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

#endif // ELEMENTO_ARISTA_H
