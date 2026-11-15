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
    TextoFlotante(QPointF posicion, QString texto, QColor color);

    QPointF getPosicion() const;
    QString getTexto() const;
    QColor getColor() const;
    double getEdad() const;
    void avanzarEdad(double dt);
};
