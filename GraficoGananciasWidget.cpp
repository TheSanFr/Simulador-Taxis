#pragma once

#include <QChart>
#include <QChartView>
#include <QColor>
#include <QLegend>
#include <QLineSeries>
#include <QMargins>
#include <QPainter>
#include <QPen>
#include <QString>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QWidget>

#include "Simulacion.cpp"
#include "Taxi.cpp"

class GraficoGananciasWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    QChart *grafico;
    QChartView *vista;
    QValueAxis *ejeX;
    QValueAxis *ejeY;
    QLineSeries *series[7];
    int cantidadSeries;

private slots:
    void reconstruirSeries()
    {
        for (int i = 0; i < cantidadSeries; i++)
        {
            grafico->removeSeries(series[i]);
            delete series[i];
            series[i] = nullptr;
        }
        cantidadSeries = 0;

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

        ejeX->setRange(0, 90);
        ejeY->setRange(0, 50000);
    }

    void enMuestra(double segundos)
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

public:
    explicit GraficoGananciasWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        sim = nullptr;
        cantidadSeries = 0;
        for (int i = 0; i < 7; i++)
        {
            series[i] = nullptr;
        }

        grafico = new QChart();
        grafico->setTheme(QChart::ChartThemeDark);
        grafico->setBackgroundBrush(QColor("#0c121b"));
        grafico->setTitle(QStringLiteral("Ingresos acumulados por taxi"));
        grafico->setTitleBrush(QColor("#ffd60a"));
        grafico->legend()->setAlignment(Qt::AlignBottom);
        grafico->setMargins(QMargins(4, 4, 4, 4));

        ejeX = new QValueAxis();
        ejeX->setTitleText(QStringLiteral("Tiempo simulado (s)"));
        ejeX->setLabelFormat("%.0f");
        ejeX->setRange(0, 90);

        ejeY = new QValueAxis();
        ejeY->setLabelFormat("%.0f");
        ejeY->setRange(0, 50000);

        grafico->addAxis(ejeX, Qt::AlignBottom);
        grafico->addAxis(ejeY, Qt::AlignLeft);

        vista = new QChartView(grafico, this);
        vista->setRenderHint(QPainter::Antialiasing);
        vista->setBackgroundBrush(QColor("#0c121b"));

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(vista);
    }

    void setSimulacion(Simulacion *nuevaSim)
    {
        sim = nuevaSim;
        connect(sim, &Simulacion::muestraGrafico, this, &GraficoGananciasWidget::enMuestra);
        connect(sim, &Simulacion::reinicioCompletado, this, &GraficoGananciasWidget::reconstruirSeries);
        reconstruirSeries();
    }
};

#ifdef COMPILAR_MOCS
#include "GraficoGananciasWidget.moc"
#endif
