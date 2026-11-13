#pragma once

#include <QPolygonF>
#include <QString>

class ManzanaCiudad
{
private:
    QPolygonF poligono;
    QString tipo;

public:
    ManzanaCiudad(QPolygonF poligono, QString tipo)
    {
        this->poligono = poligono;
        this->tipo = tipo;
    }

    QPolygonF getPoligono() const { return poligono; }
    QString getTipo() const { return tipo; }
};
