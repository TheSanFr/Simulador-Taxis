#include "ResultadoRuta.h"

ResultadoRuta::ResultadoRuta()
{
    encontrada = false;
    cantidadNodos = 0;
    tiempoPonderado = 0.0;
    nodosExplorados = 0;
}

bool ResultadoRuta::getEncontrada() const
{
    return encontrada;
}

int ResultadoRuta::getCantidadNodos() const
{
    return cantidadNodos;
}

int ResultadoRuta::getNodoEn(int indice) const
{
    return camino[indice];
}

double ResultadoRuta::getTiempoPonderado() const
{
    return tiempoPonderado;
}

int ResultadoRuta::getNodosExplorados() const
{
    return nodosExplorados;
}

int ResultadoRuta::getSaltos() const
{
    if (cantidadNodos <= 1)
    {
        return 0;
    }
    return cantidadNodos - 1;
}

void ResultadoRuta::setEncontrada(bool encontrada)
{
    this->encontrada = encontrada;
}

void ResultadoRuta::setTiempoPonderado(double tiempoPonderado)
{
    this->tiempoPonderado = tiempoPonderado;
}

void ResultadoRuta::setNodosExplorados(int nodosExplorados)
{
    this->nodosExplorados = nodosExplorados;
}

void ResultadoRuta::limpiarCamino()
{
    cantidadNodos = 0;
}

void ResultadoRuta::agregarNodoAlCamino(int idNodo)
{
    if (cantidadNodos < 100)
    {
        camino[cantidadNodos] = idNodo;
        cantidadNodos++;
    }
}
