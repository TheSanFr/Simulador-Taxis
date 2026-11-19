#include "interfaz/BarrasGananciasWidget.h"

#include "simulacion/Grafo.h"
#include "simulacion/Simulacion.h"
#include "taxis/Taxi.h"

#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QRectF>

void BarrasGananciasWidget::reiniciarBarras()
{
    for (int i = 0; i < 7; i++)
    {
        mostrado[i] = 0.0;
    }
    escala = 50000.0;
    if (isVisible())
        update();
}

void BarrasGananciasWidget::enCuadro(double dt)
{
    if (sim == nullptr || !isVisible())
    {
        return;
    }

    double maximo = 50000.0;
    const double factor = qMin(dt * 5.0, 1.0);
    const double tolerancia = escala * 0.0005;
    bool cambio = false;

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        const double meta = sim->getTaxi(i)->getIngresos();
        const double diferencia = meta - mostrado[i];
        const double valor = qAbs(diferencia) < tolerancia ? meta : mostrado[i] + diferencia * factor;
        cambio |= valor != mostrado[i];
        mostrado[i] = valor;
        maximo = qMax(maximo, valor * 1.15);
    }

    const double diferencia = maximo - escala;
    const double siguienteEscala = qAbs(diferencia) < tolerancia
        ? maximo : escala + diferencia * qMin(dt * 3.0, 1.0);
    if (cambio || siguienteEscala != escala)
    {
        escala = siguienteEscala;
        update();
    }
}

void BarrasGananciasWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (sim == nullptr)
        return;

    escala = 50000.0;
    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        mostrado[i] = sim->getTaxi(i)->getIngresos();
        escala = qMax(escala, mostrado[i] * 1.15);
    }
}

void BarrasGananciasWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#2b3a49"), 1));
    p.setBrush(QColor("#131d28"));
    p.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), 8, 8);

    if (sim == nullptr || sim->getCantidadTaxis() == 0)
    {
        return;
    }

    int n = sim->getCantidadTaxis();
    double margen = 10.0;
    double anchoEtiqueta = 86.0;
    double anchoValor = 98.0;
    double altoFila = (height() - 2 * margen) / n;
    double altoBarra = qMin(altoFila - 8.0, 12.0);
    double xPista = margen + anchoEtiqueta;
    double anchoPista = width() - 2 * margen - anchoEtiqueta - anchoValor;
    if (anchoPista < 40.0) anchoPista = 40.0;

    int lider = 0;
    for (int i = 1; i < n; i++)
    {
        if (sim->getTaxi(i)->getIngresos() > sim->getTaxi(lider)->getIngresos())
        {
            lider = i;
        }
    }

    QFont fuente = p.font();
    fuente.setPixelSize(13);
    fuente.setWeight(QFont::DemiBold);
    p.setFont(fuente);

    for (int i = 0; i < n; i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        const double ingresos = taxi->getIngresos();
        const bool destacado = i == lider && ingresos > 0.0;
        double cy = margen + altoFila * (i + 0.5);
        QColor c = taxi->getColor();

        p.setPen(Qt::NoPen);
        p.setBrush(c);
        p.drawEllipse(QPointF(margen + 6, cy), 4.0, 4.0);
        p.setPen(QColor("#e8eef4"));
        p.drawText(QRectF(margen + 18, cy - 11, anchoEtiqueta - 18, 22),
                   Qt::AlignVCenter | Qt::AlignLeft, taxi->getNombre());

        QRectF pista(xPista, cy - altoBarra / 2, anchoPista, altoBarra);
        p.setPen(destacado ? QPen(QColor("#ffd166"), 1.0) : Qt::NoPen);
        p.setBrush(QColor("#263645"));
        p.drawRoundedRect(pista, 5, 5);

        double fraccion = mostrado[i] / escala;
        if (fraccion < 0.0) fraccion = 0.0;
        if (fraccion > 1.0) fraccion = 1.0;

        if (fraccion > 0.002)
        {
            double anchoRelleno = anchoPista * fraccion;
            if (anchoRelleno < altoBarra) anchoRelleno = altoBarra;
            QRectF relleno(xPista, cy - altoBarra / 2, anchoRelleno, altoBarra);
            p.setPen(Qt::NoPen);
            p.setBrush(c);
            p.drawRoundedRect(relleno, 5, 5);
        }

        p.setPen(destacado ? QColor("#ffd166") : QColor("#d7e2ec"));
        p.drawText(QRectF(width() - margen - anchoValor, cy - 11, anchoValor, 22),
                   Qt::AlignVCenter | Qt::AlignRight, formatearPesos(ingresos));
    }
}

BarrasGananciasWidget::BarrasGananciasWidget(QWidget *parent) : QWidget(parent)
{
    sim = nullptr;
    for (int i = 0; i < 7; i++)
    {
        mostrado[i] = 0.0;
    }
    escala = 50000.0;
    setMinimumHeight(190);
    setMaximumHeight(248);
}

void BarrasGananciasWidget::setSimulacion(Simulacion *nuevaSim)
{
    if (sim == nuevaSim)
        return;

    sim = nuevaSim;
    reiniciarBarras();
    if (sim == nullptr)
        return;

    if (isVisible())
    {
        escala = 50000.0;
        for (int i = 0; i < sim->getCantidadTaxis(); i++)
        {
            mostrado[i] = sim->getTaxi(i)->getIngresos();
            escala = qMax(escala, mostrado[i] * 1.15);
        }
    }
}
