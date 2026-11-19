#ifndef RELOJ_SIMULACION_H
#define RELOJ_SIMULACION_H

#include <QString>

class RelojSimulacion
{
private:
    double segundosDelDia;
    int dia;
    int modo;

    int minutoActual() const;

public:
    RelojSimulacion();

    void reiniciar();

    void avanzar(double segundosSimulados);

    int getDia() const;
    int getModo() const;
    void setModo(int modo);

    QString textoHora() const;

    QString periodoActual() const;
    QString textoProximoPeriodo() const;

    double oscuridad() const;
};

#endif
