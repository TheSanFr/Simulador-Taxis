#pragma once

class ResultadoRuta
{
private:
    bool encontrada;
    int camino[100];
    int cantidadNodos;
    double tiempoPonderado;
    int nodosExplorados;

public:
    ResultadoRuta()
    {
        encontrada = false;
        cantidadNodos = 0;
        tiempoPonderado = 0.0;
        nodosExplorados = 0;
    }

    bool getEncontrada() const { return encontrada; }
    int getCantidadNodos() const { return cantidadNodos; }
    int getNodoEn(int indice) const { return camino[indice]; }
    double getTiempoPonderado() const { return tiempoPonderado; }
    int getNodosExplorados() const { return nodosExplorados; }

    int getSaltos() const
    {
        if (cantidadNodos <= 1)
        {
            return 0;
        }
        return cantidadNodos - 1;
    }

    void setEncontrada(bool encontrada) { this->encontrada = encontrada; }
    void setTiempoPonderado(double tiempoPonderado) { this->tiempoPonderado = tiempoPonderado; }
    void setNodosExplorados(int nodosExplorados) { this->nodosExplorados = nodosExplorados; }

    void limpiarCamino()
    {
        cantidadNodos = 0;
    }

    void agregarNodoAlCamino(int idNodo)
    {
        if (cantidadNodos < 100)
        {
            camino[cantidadNodos] = idNodo;
            cantidadNodos++;
        }
    }
};
