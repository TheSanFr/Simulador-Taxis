#pragma once

class ElementoNodo;
class Nodo;

class ListaNodos
{
private:
    ElementoNodo *primero;
    int cantidad;

public:
    ListaNodos();

    ~ListaNodos();

    void agregar(Nodo *nodo);

    Nodo *buscarPorId(int id) const;

    bool eliminarPorId(int id);

    ElementoNodo *getPrimero() const;
    int getCantidad() const;

    void limpiar();
};
