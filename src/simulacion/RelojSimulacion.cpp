#include "simulacion/RelojSimulacion.h"

#include <cmath>

namespace
{
const double segundosPorDia = 720.0;
const double minutosPorSegundo = 1440.0 / segundosPorDia;
const double toleranciaSegundos = 0.000000001;

struct CambioPeriodo
{
    int minuto;
    QString periodo;
    QString nombre;
};

const int cantidadCambios = 8;
const CambioPeriodo agenda[cantidadCambios] = {
    {5 * 60, "Normal", "Normal"},
    {6 * 60 + 30, "HoraPico", "Pico"},
    {9 * 60, "Normal", "Normal"},
    {11 * 60 + 30, "HoraPico", "Pico"},
    {14 * 60, "Normal", "Normal"},
    {17 * 60, "HoraPico", "Pico"},
    {19 * 60 + 30, "Normal", "Normal"},
    {21 * 60, "Nocturno", "Nocturno"},
};

QString formatearHora(int minuto)
{
    return QStringLiteral("%1:%2")
        .arg(minuto / 60, 2, 10, QLatin1Char('0'))
        .arg(minuto % 60, 2, 10, QLatin1Char('0'));
}
}

RelojSimulacion::RelojSimulacion()
    : modo(0)
{
    reiniciar();
}

void RelojSimulacion::reiniciar()
{
    segundosDelDia = agenda[0].minuto / minutosPorSegundo;
    dia = 1;
}

void RelojSimulacion::avanzar(double segundosSimulados)
{
    if (!std::isfinite(segundosSimulados) || segundosSimulados <= 0.0)
    {
        return;
    }
    segundosDelDia += segundosSimulados;
    while (segundosDelDia >= segundosPorDia - toleranciaSegundos)
    {
        segundosDelDia -= segundosPorDia;
        if (segundosDelDia < 0.0)
            segundosDelDia = 0.0;
        dia++;
    }
}

int RelojSimulacion::getDia() const
{
    return dia;
}

int RelojSimulacion::getModo() const
{
    return modo;
}

void RelojSimulacion::setModo(int modo)
{
    if (modo >= 0 && modo <= 3)
    {
        this->modo = modo;
    }
}

QString RelojSimulacion::textoHora() const
{
    return formatearHora(minutoActual());
}

int RelojSimulacion::minutoActual() const
{
    return static_cast<int>((segundosDelDia + toleranciaSegundos) * minutosPorSegundo);
}

QString RelojSimulacion::periodoActual() const
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

    const int minuto = minutoActual();
    QString periodo = "Nocturno";
    for (int i = 0; i < cantidadCambios; i++)
    {
        if (minuto < agenda[i].minuto)
        {
            break;
        }
        periodo = agenda[i].periodo;
    }
    return periodo;
}

QString RelojSimulacion::textoProximoPeriodo() const
{
    if (modo != 0)
        return {};

    const int minuto = minutoActual();
    int indiceProximoCambio = 0;
    for (int i = 0; i < cantidadCambios; i++)
    {
        if (minuto < agenda[i].minuto)
        {
            indiceProximoCambio = i;
            break;
        }
    }
    return QStringLiteral("%1 a las %2").arg(agenda[indiceProximoCambio].nombre,
        formatearHora(agenda[indiceProximoCambio].minuto));
}

double RelojSimulacion::oscuridad() const
{
    if (modo == 2)
    {
        return 1.0;
    }
    if (modo == 1 || modo == 3)
    {
        return 0.0;
    }

    const double hora = segundosDelDia * (24.0 / segundosPorDia);
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
