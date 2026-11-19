#include "ManzanaCiudad.h"

ManzanaCiudad::ManzanaCiudad(QPolygonF poligono, QString tipo)
{
    this->poligono = poligono;
    this->tipo = tipo;
}

QPolygonF ManzanaCiudad::getPoligono() const
{
    return poligono;
}

QString ManzanaCiudad::getTipo() const
{
    return tipo;
}
