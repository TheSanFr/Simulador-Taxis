#pragma once

#include <QPolygonF>
#include <QString>

class ManzanaCiudad
{
private:
    QPolygonF poligono;
    QString tipo;

public:
    ManzanaCiudad(QPolygonF poligono, QString tipo);

    QPolygonF getPoligono() const;
    QString getTipo() const;
};
