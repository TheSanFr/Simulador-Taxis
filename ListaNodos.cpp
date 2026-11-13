#pragma once

#include "ElementoNodo.cpp"
#include "Nodo.cpp"

class ListaNodos
{
private:
    ElementoNodo *primero;
    int cantidad;

public:
    ListaNodos()
    {
        primero = nullptr;
        cantidad = 0;
    }

    ~ListaNodos()
    {
        limpiar();
    }

    void agregar(Nodo *nodo)
    {
        if (nodo == nullptr)
        {
            return;
        }

        ElementoNodo *nuevo = new ElementoNodo(nodo);

        if (primero == nullptr)
        {
            primero = nuevo;
        }
        else
        {
            ElementoNodo *actual = primero;
            while (actual->getSiguiente() != nullptr)
            {
                actual = actual->getSiguiente();
            }
            actual->setSiguiente(nuevo);
        }

        cantidad++;
    }

    Nodo *buscarPorId(int id) const
    {
        ElementoNodo *actual = primero;

        while (actual != nullptr)
        {
            Nodo *nodo = actual->getNodo();
            if (nodo != nullptr && nodo->getId() == id)
            {
                return nodo;
            }
            actual = actual->getSiguiente();
        }

        return nullptr;
    }

    bool eliminarPorId(int id)
    {
        ElementoNodo *actual = primero;
        ElementoNodo *anterior = nullptr;

        while (actual != nullptr)
        {
            Nodo *nodo = actual->getNodo();

            if (nodo != nullptr && nodo->getId() == id)
            {
                if (anterior == nullptr)
                {
                    primero = actual->getSiguiente();
                }
                else
                {
                    anterior->setSiguiente(actual->getSiguiente());
                }

                delete actual->getNodo();
                delete actual;
                cantidad--;
                return true;
            }

            anterior = actual;
            actual = actual->getSiguiente();
        }

        return false;
    }

    ElementoNodo *getPrimero() const { return primero; }
    int getCantidad() const { return cantidad; }

    void limpiar()
    {
        ElementoNodo *actual = primero;

        while (actual != nullptr)
        {
            ElementoNodo *siguiente = actual->getSiguiente();
            delete actual->getNodo();
            delete actual;
            actual = siguiente;
        }

        primero = nullptr;
        cantidad = 0;
    }
};
