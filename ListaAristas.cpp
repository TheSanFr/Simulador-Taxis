#include "ListaAristas.h"

#include "Arista.h"
#include "ElementoArista.h"

ListaAristas::ListaAristas()
{
    primero = nullptr;
    cantidad = 0;
}

ListaAristas::~ListaAristas()
{
    limpiar();
}

void ListaAristas::agregar(Arista *arista)
{
    if (arista == nullptr)
    {
        return;
    }

    ElementoArista *nuevo = new ElementoArista(arista);

    if (primero == nullptr)
    {
        primero = nuevo;
    }
    else
    {
        ElementoArista *actual = primero;
        while (actual->getSiguiente() != nullptr)
        {
            actual = actual->getSiguiente();
        }
        actual->setSiguiente(nuevo);
    }

    cantidad++;
}

Arista *ListaAristas::obtenerArista(int origen, int destino) const
{
    ElementoArista *actual = primero;

    while (actual != nullptr)
    {
        Arista *arista = actual->getArista();
        if (arista != nullptr && arista->getOrigen() == origen && arista->getDestino() == destino)
        {
            return arista;
        }
        actual = actual->getSiguiente();
    }

    return nullptr;
}

bool ListaAristas::existeArista(int origen, int destino) const
{
    return obtenerArista(origen, destino) != nullptr;
}

bool ListaAristas::eliminarArista(int origen, int destino)
{
    ElementoArista *actual = primero;
    ElementoArista *anterior = nullptr;

    while (actual != nullptr)
    {
        Arista *arista = actual->getArista();

        if (arista != nullptr && arista->getOrigen() == origen && arista->getDestino() == destino)
        {
            if (anterior == nullptr)
            {
                primero = actual->getSiguiente();
            }
            else
            {
                anterior->setSiguiente(actual->getSiguiente());
            }

            delete actual->getArista();
            delete actual;
            cantidad--;
            return true;
        }

        anterior = actual;
        actual = actual->getSiguiente();
    }

    return false;
}

void ListaAristas::eliminarAristasDeNodo(int nodoId)
{
    ElementoArista *actual = primero;
    ElementoArista *anterior = nullptr;

    while (actual != nullptr)
    {
        ElementoArista *siguiente = actual->getSiguiente();
        Arista *arista = actual->getArista();
        bool eliminar = false;

        if (arista != nullptr)
        {
            if (arista->getOrigen() == nodoId || arista->getDestino() == nodoId)
            {
                eliminar = true;
            }
        }

        if (eliminar)
        {
            if (anterior == nullptr)
            {
                primero = siguiente;
            }
            else
            {
                anterior->setSiguiente(siguiente);
            }

            delete actual->getArista();
            delete actual;
            cantidad--;
        }
        else
        {
            anterior = actual;
        }

        actual = siguiente;
    }
}

ElementoArista *ListaAristas::getPrimero() const
{
    return primero;
}

int ListaAristas::getCantidad() const
{
    return cantidad;
}

void ListaAristas::limpiar()
{
    ElementoArista *actual = primero;

    while (actual != nullptr)
    {
        ElementoArista *siguiente = actual->getSiguiente();
        delete actual->getArista();
        delete actual;
        actual = siguiente;
    }

    primero = nullptr;
    cantidad = 0;
}
