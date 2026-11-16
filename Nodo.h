#ifndef NODO_H
#define NODO_H

#include <QPointF>
#include <QString>

class Nodo
{
private:
    int id;
    QString nombre;
    int x;
    int y;
    QString tipo; // "Normal", "Aeropuerto", "Terminal", "Turistico"

public:
    Nodo(int id, QString nombre, int x, int y, QString tipo);

    int getId() const;
    QString getNombre() const;
    int getX() const;
    int getY() const;
    QPointF getPosicion() const;
    QString getTipo() const;

    void setId(int id);
    void setNombre(QString nombre);
    void setX(int x);
    void setY(int y);
    void setTipo(QString tipo);
};

#endif // NODO_H
