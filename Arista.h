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
    Arista(int origen, int destino, double peso, bool esAvenida);

    int getOrigen() const;
    int getDestino() const;
    double getPeso() const;
    double getPesoInicial() const;
    bool getEsAvenida() const;

    void setOrigen(int origen);
    void setDestino(int destino);
    void setPeso(double peso);
    void setPesoInicial(double pesoInicial);
    void setEsAvenida(bool esAvenida);
};
