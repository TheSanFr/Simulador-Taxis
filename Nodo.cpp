#include "Nodo.h"

Nodo::Nodo(int id, QString nombre, int x, int y, QString tipo)
{
    this->id = id;
    this->nombre = nombre;
    this->x = x;
    this->y = y;
    this->tipo = tipo;
}

int Nodo::getId() const
{
    return id;
}

QString Nodo::getNombre() const
{
    return nombre;
}

int Nodo::getX() const
{
    return x;
}

int Nodo::getY() const
{
    return y;
}

QPointF Nodo::getPosicion() const
{
    return QPointF(x, y);
}

QString Nodo::getTipo() const
{
    return tipo;
}

void Nodo::setId(int id)
{
    this->id = id;
}

void Nodo::setNombre(QString nombre)
{
    this->nombre = nombre;
}

void Nodo::setX(int x)
{
    this->x = x;
}

void Nodo::setY(int y)
{
    this->y = y;
}

void Nodo::setTipo(QString tipo)
{
    this->tipo = tipo;
}
