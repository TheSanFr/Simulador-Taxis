#pragma once

class Arista
{
private:
    int origen;
    int destino;
    double peso;
    double pesoInicial;
    bool esAvenida;

public:
    Arista(int origen, int destino, double peso, bool esAvenida)
    {
        this->origen = origen;
        this->destino = destino;
        this->peso = peso;
        this->pesoInicial = peso;
        this->esAvenida = esAvenida;
    }

    int getOrigen() const { return origen; }
    int getDestino() const { return destino; }
    double getPeso() const { return peso; }
    double getPesoInicial() const { return pesoInicial; }
    bool getEsAvenida() const { return esAvenida; }

    void setOrigen(int origen) { this->origen = origen; }
    void setDestino(int destino) { this->destino = destino; }
    void setPeso(double peso) { this->peso = peso; }
    void setPesoInicial(double pesoInicial) { this->pesoInicial = pesoInicial; }
    void setEsAvenida(bool esAvenida) { this->esAvenida = esAvenida; }
};
