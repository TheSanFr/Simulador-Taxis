#include "RelojSimulacion.h"

namespace
{
constexpr double segundosPorDia = 720.0;
constexpr double minutosPorSegundo = 1440.0 / segundosPorDia;
constexpr double toleranciaSegundos = 1e-9;

struct CambioPeriodo
{
    int minuto;
    const char *periodo;
    const char *nombre;
};

constexpr CambioPeriodo agenda[] = {
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
    this->modo = modo;
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
    const char *periodo = "Nocturno";
    for (const CambioPeriodo &cambio : agenda)
    {
        if (minuto < cambio.minuto)
            break;
        periodo = cambio.periodo;
    }
    return QString::fromLatin1(periodo);
}

QString RelojSimulacion::textoProximoPeriodo() const
{
    if (modo != 0)
        return {};

    const int minuto = minutoActual();
    const CambioPeriodo *proximo = &agenda[0];
    for (const CambioPeriodo &cambio : agenda)
    {
        if (minuto < cambio.minuto)
        {
            proximo = &cambio;
            break;
        }
    }
    return QStringLiteral("%1 a las %2").arg(QString::fromLatin1(proximo->nombre),
                                             formatearHora(proximo->minuto));
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
