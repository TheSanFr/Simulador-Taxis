#ifndef LISTA_ARISTAS_H
#define LISTA_ARISTAS_H

class Arista;
class ElementoArista;

class ListaAristas
{
private:
    ElementoArista *primero;
    int cantidad;

public:
    ListaAristas();

    ~ListaAristas();

    void agregar(Arista *arista);

    Arista *obtenerArista(int origen, int destino) const;

    bool existeArista(int origen, int destino) const;

    bool eliminarArista(int origen, int destino);

    void eliminarAristasDeNodo(int nodoId);

    ElementoArista *getPrimero() const;
    int getCantidad() const;

    void limpiar();
};

#endif // LISTA_ARISTAS_H
