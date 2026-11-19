#include "simulacion/RelojSimulacion.h"

#include <cmath>

const double segundosPorDia = 720.0;
const double minutosPorSegundo = 1440.0 / segundosPorDia;
const double toleranciaSegundos = 0.000000001;

const int cantidadCambios = 8;
const int agendaMinutos[cantidadCambios] = {
    5 * 60,
    6 * 60 + 30,
    9 * 60,
    11 * 60 + 30,
    14 * 60,
    17 * 60,
    19 * 60 + 30,
    21 * 60
};

const char *agendaPeriodos[cantidadCambios] = {
    "Normal",
    "HoraPico",
    "Normal",
    "HoraPico",
    "Normal",
    "HoraPico",
    "Normal",
    "Nocturno"
};

const char *agendaNombres[cantidadCambios] = {
    "Normal",
    "Pico",
    "Normal",
    "Pico",
    "Normal",
    "Pico",
    "Normal",
    "Nocturno"
};

QString formatearHoraReloj(int minuto)
{
    return QStringLiteral("%1:%2")
        .arg(minuto / 60, 2, 10, QLatin1Char('0'))
        .arg(minuto % 60, 2, 10, QLatin1Char('0'));
}

RelojSimulacion::RelojSimulacion()
    : modo(0)
{
    reiniciar();
}

void RelojSimulacion::reiniciar()
{
    segundosDelDia = agendaMinutos[0] / minutosPorSegundo;
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
    return formatearHoraReloj(minutoActual());
}

int RelojSimulacion::minutoActual() const
{
    return int((segundosDelDia + toleranciaSegundos) * minutosPorSegundo);
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
        if (minuto < agendaMinutos[i])
        {
            break;
        }
        periodo = QString::fromUtf8(agendaPeriodos[i]);
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
        if (minuto < agendaMinutos[i])
        {
            indiceProximoCambio = i;
            break;
        }
    }
    return QStringLiteral("%1 a las %2").arg(
        QString::fromUtf8(agendaNombres[indiceProximoCambio]),
        formatearHoraReloj(agendaMinutos[indiceProximoCambio]));
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
