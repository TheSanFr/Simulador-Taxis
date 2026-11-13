#pragma once

#include <QColor>
#include <QPointF>
#include <QString>

class TextoFlotante
{
private:
    QPointF posicion;
    QString texto;
    QColor color;
    double edad;

public:
    TextoFlotante(QPointF posicion, QString texto, QColor color)
    {
        this->posicion = posicion;
        this->texto = texto;
        this->color = color;
        this->edad = 0.0;
    }

    QPointF getPosicion() const { return posicion; }
    QString getTexto() const { return texto; }
    QColor getColor() const { return color; }
    double getEdad() const { return edad; }
    void avanzarEdad(double dt) { edad += dt; }
};
