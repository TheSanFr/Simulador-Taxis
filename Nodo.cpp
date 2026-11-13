#pragma once

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
    Nodo(int id, QString nombre, int x, int y, QString tipo)
    {
        this->id = id;
        this->nombre = nombre;
        this->x = x;
        this->y = y;
        this->tipo = tipo;
    }

    int getId() const { return id; }
    QString getNombre() const { return nombre; }
    int getX() const { return x; }
    int getY() const { return y; }
    QPointF getPosicion() const { return QPointF(x, y); }
    QString getTipo() const { return tipo; }

    void setId(int id) { this->id = id; }
    void setNombre(QString nombre) { this->nombre = nombre; }
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setTipo(QString tipo) { this->tipo = tipo; }
};
