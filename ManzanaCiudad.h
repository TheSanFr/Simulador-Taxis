#ifndef MANZANA_CIUDAD_H
#define MANZANA_CIUDAD_H

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

#endif
