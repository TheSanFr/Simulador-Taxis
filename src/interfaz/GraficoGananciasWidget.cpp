#include "interfaz/GraficoGananciasWidget.h"

#include "simulacion/Simulacion.h"
#include "taxis/Taxi.h"

#include <QChart>
#include <QChartView>
#include <QColor>
#include <QFrame>
#include <QLegend>
#include <QLineSeries>
#include <QMargins>
#include <QPainter>
#include <QPen>
#include <QString>
#include <QVBoxLayout>
#include <QValueAxis>

void GraficoGananciasWidget::reconstruirSeries()
{
    for (int i = 0; i < cantidadSeries; i++)
    {
        grafico->removeSeries(series[i]);
        delete series[i];
        series[i] = nullptr;
    }
    cantidadSeries = 0;
    ejeX->setRange(0, 90);
    ejeY->setRange(0, 50000);

    if (sim == nullptr)
    {
        return;
    }

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        QLineSeries *s = new QLineSeries();
        s->setName(taxi->getNombre());
        QPen pluma(taxi->getColor());
        pluma.setWidthF(taxi->esManual() ? 3.5 : 2.2);
        s->setPen(pluma);
        grafico->addSeries(s);
        s->attachAxis(ejeX);
        s->attachAxis(ejeY);
        series[cantidadSeries] = s;
        cantidadSeries++;
    }
}

void GraficoGananciasWidget::enMuestra(double segundos)
{
    if (sim == nullptr)
    {
        return;
    }
    if (cantidadSeries != sim->getCantidadTaxis())
    {
        reconstruirSeries();
    }

    double maxIngresos = 50000.0;
    for (int i = 0; i < cantidadSeries; i++)
    {
        Taxi *tx = sim->getTaxi(i);
        series[i]->append(segundos, tx->getIngresos());
        while (series[i]->count() > 1 && series[i]->at(0).x() < segundos - 90.0)
        {
            series[i]->remove(0);
        }
        if (tx->getIngresos() * 1.15 > maxIngresos)
        {
            maxIngresos = tx->getIngresos() * 1.15;
        }
    }

    double x0 = segundos - 90.0;
    if (x0 < 0.0) x0 = 0.0;
    ejeX->setRange(x0, x0 + 90.0);
    ejeY->setRange(0, maxIngresos);
}

GraficoGananciasWidget::GraficoGananciasWidget(QWidget *parent) : QWidget(parent)
{
    sim = nullptr;
    cantidadSeries = 0;
    for (int i = 0; i < 7; i++)
    {
        series[i] = nullptr;
    }

    grafico = new QChart();
    grafico->setTheme(QChart::ChartThemeDark);
    grafico->setBackgroundBrush(QColor("#131d28"));
    grafico->setBackgroundRoundness(9);
    grafico->setPlotAreaBackgroundVisible(true);
    grafico->setPlotAreaBackgroundBrush(QColor("#17222e"));
    grafico->setTitle(QStringLiteral("Ingresos acumulados por taxi"));
    grafico->setTitleBrush(QColor("#e8eef4"));
    grafico->legend()->setLabelColor(QColor("#b8c8d8"));
    grafico->legend()->setAlignment(Qt::AlignRight);
    grafico->setMargins(QMargins(4, 4, 4, 4));

    ejeX = new QValueAxis();
    ejeX->setTitleText(QStringLiteral("Tiempo simulado (s)"));
    ejeX->setTickCount(5);
    ejeX->setLabelFormat("%.0f");
    ejeX->setRange(0, 90);

    ejeY = new QValueAxis();
    ejeY->setTickCount(5);
    ejeY->setLabelFormat("%.0f");
    ejeY->setRange(0, 50000);

    grafico->addAxis(ejeX, Qt::AlignBottom);
    grafico->addAxis(ejeY, Qt::AlignLeft);

    ejeX->setLabelsColor(QColor("#a5b4c3"));
    ejeX->setGridLineColor(QColor("#2b3a49"));
    ejeX->setLinePenColor(QColor("#526579"));
    ejeX->setTitleBrush(QColor("#c5d2de"));

    ejeY->setLabelsColor(QColor("#a5b4c3"));
    ejeY->setGridLineColor(QColor("#2b3a49"));
    ejeY->setLinePenColor(QColor("#526579"));
    ejeY->setTitleBrush(QColor("#c5d2de"));

    vista = new QChartView(grafico, this);
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setBackgroundBrush(QColor("#17222e"));
    vista->setFrameShape(QFrame::NoFrame);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(vista);
}

void GraficoGananciasWidget::setSimulacion(Simulacion *nuevaSim)
{
    if (sim == nuevaSim)
        return;

    sim = nuevaSim;
    reconstruirSeries();
}
