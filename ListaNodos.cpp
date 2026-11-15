#include "ListaNodos.h"

#include "ElementoNodo.h"
#include "Nodo.h"

ListaNodos::ListaNodos()
{
    primero = nullptr;
    cantidad = 0;
}

ListaNodos::~ListaNodos()
{
    limpiar();
}

void ListaNodos::agregar(Nodo *nodo)
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

Nodo *ListaNodos::buscarPorId(int id) const
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

bool ListaNodos::eliminarPorId(int id)
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

ElementoNodo *ListaNodos::getPrimero() const
{
    return primero;
}

int ListaNodos::getCantidad() const
{
    return cantidad;
}

void ListaNodos::limpiar()
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
