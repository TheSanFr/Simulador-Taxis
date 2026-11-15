#pragma once

#include <QString>

class RelojSimulacion
{
private:
    double hora;
    int dia;
    int modo; // 0 = Automático, 1 = Normal, 2 = Nocturno, 3 = Hora Pico

public:
    RelojSimulacion();

    void reiniciar();

    void avanzar(double segundosSimulados);

    int getDia() const;
    int getModo() const;
    void setModo(int modo);

    QString textoHora() const;

    QString periodoActual() const;

    double oscuridad() const;
};
