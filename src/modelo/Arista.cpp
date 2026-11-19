#include "modelo/Arista.h"

Arista::Arista(int origen, int destino, double peso, bool esAvenida)
{
    this->origen = origen;
    this->destino = destino;
    this->peso = peso;
    this->pesoInicial = peso;
    this->esAvenida = esAvenida;
}

int Arista::getOrigen() const
{
    return origen;
}

int Arista::getDestino() const
{
    return destino;
}

double Arista::getPeso() const
{
    return peso;
}

double Arista::getPesoInicial() const
{
    return pesoInicial;
}

bool Arista::getEsAvenida() const
{
    return esAvenida;
}

void Arista::setOrigen(int origen)
{
    this->origen = origen;
}

void Arista::setDestino(int destino)
{
    this->destino = destino;
}

void Arista::setPeso(double peso)
{
    this->peso = peso;
}

void Arista::setPesoInicial(double pesoInicial)
{
    this->pesoInicial = pesoInicial;
}

void Arista::setEsAvenida(bool esAvenida)
{
    this->esAvenida = esAvenida;
}
