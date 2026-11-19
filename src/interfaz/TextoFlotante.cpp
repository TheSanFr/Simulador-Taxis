#include "interfaz/TextoFlotante.h"

TextoFlotante::TextoFlotante(QPointF posicion, QString texto, QColor color)
{
    this->posicion = posicion;
    this->texto = texto;
    this->color = color;
    this->edad = 0.0;
}

QPointF TextoFlotante::getPosicion() const
{
    return posicion;
}

QString TextoFlotante::getTexto() const
{
    return texto;
}

QColor TextoFlotante::getColor() const
{
    return color;
}

double TextoFlotante::getEdad() const
{
    return edad;
}

void TextoFlotante::avanzarEdad(double dt)
{
    edad += dt;
}
