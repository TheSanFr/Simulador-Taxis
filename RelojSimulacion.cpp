#pragma once

#include <QString>

class RelojSimulacion
{
private:
    double hora;
    int dia;
    int modo; // 0 = Automático, 1 = Normal, 2 = Nocturno, 3 = Hora Pico

public:
    RelojSimulacion()
    {
        hora = 6.0;
        dia = 1;
        modo = 0;
    }

    void reiniciar()
    {
        hora = 6.0;
        dia = 1;
    }

    void avanzar(double segundosSimulados)
    {
        hora += segundosSimulados * (24.0 / 240.0);
        while (hora >= 24.0)
        {
            hora -= 24.0;
            dia++;
        }
    }

    int getDia() const { return dia; }
    int getModo() const { return modo; }
    void setModo(int modo) { this->modo = modo; }

    QString textoHora() const
    {
        int h = static_cast<int>(hora);
        int m = static_cast<int>((hora - h) * 60.0);
        return QStringLiteral("%1:%2")
            .arg(h, 2, 10, QLatin1Char('0'))
            .arg(m, 2, 10, QLatin1Char('0'));
    }

    QString periodoActual() const
    {
        if (modo == 1)
        {
            return "Normal";
        }
        if (modo == 2)
        {
            return "Nocturno";
        }
        if (modo == 3)
        {
            return "HoraPico";
        }

        if ((hora >= 6.5 && hora < 9.0) || (hora >= 11.5 && hora < 14.0) || (hora >= 17.0 && hora < 19.5))
        {
            return "HoraPico";
        }
        if (hora >= 21.0 || hora < 5.0)
        {
            return "Nocturno";
        }

        return "Normal";
    }

    double oscuridad() const
    {
        if (modo == 2)
        {
            return 1.0;
        }
        if (modo == 1 || modo == 3)
        {
            return 0.0;
        }

        if (hora >= 21.5 || hora < 5.0)
        {
            return 1.0;
        }
        if (hora >= 19.0)
        {
            return (hora - 19.0) / 2.5;
        }
        if (hora < 7.0)
        {
            return 1.0 - (hora - 5.0) / 2.0;
        }
        return 0.0;
    }
};
