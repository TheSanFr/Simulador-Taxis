#ifndef RESULTADO_RUTA_H
#define RESULTADO_RUTA_H

class ResultadoRuta
{
private:
    bool encontrada;
    int camino[100];
    int cantidadNodos;
    double tiempoPonderado;
    int nodosExplorados;

public:
    ResultadoRuta();

    bool getEncontrada() const;
    int getCantidadNodos() const;
    int getNodoEn(int indice) const;
    double getTiempoPonderado() const;
    int getNodosExplorados() const;

    int getSaltos() const;

    void setEncontrada(bool encontrada);
    void setTiempoPonderado(double tiempoPonderado);
    void setNodosExplorados(int nodosExplorados);

    void limpiarCamino();

    void agregarNodoAlCamino(int idNodo);
};

#endif // RESULTADO_RUTA_H
