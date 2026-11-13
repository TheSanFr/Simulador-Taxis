#pragma once

#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QRectF>
#include <QWidget>

#include "Grafo.cpp"
#include "Simulacion.cpp"
#include "Taxi.cpp"

class BarrasGananciasWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    double mostrado[7];
    double escala;

private slots:
    void reiniciarBarras()
    {
        for (int i = 0; i < 7; i++)
        {
            mostrado[i] = 0.0;
        }
        escala = 50000.0;
    }

    void enCuadro(double dt)
    {
        if (sim == nullptr)
        {
            return;
        }

        double maximo = 50000.0;
        double factor = dt * 5.0;
        if (factor > 1.0) factor = 1.0;

        for (int i = 0; i < sim->getCantidadTaxis(); i++)
        {
            Taxi *tx = sim->getTaxi(i);
            mostrado[i] += (tx->getIngresos() - mostrado[i]) * factor;
            if (mostrado[i] * 1.15 > maximo)
            {
                maximo = mostrado[i] * 1.15;
            }
        }

        double factorEscala = dt * 3.0;
        if (factorEscala > 1.0) factorEscala = 1.0;
        escala += (maximo - escala) * factorEscala;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#0c121b"));
        p.drawRoundedRect(rect(), 10, 10);

        if (sim == nullptr || sim->getCantidadTaxis() == 0)
        {
            return;
        }

        int n = sim->getCantidadTaxis();
        double margen = 12.0;
        double anchoEtiqueta = 92.0;
        double anchoValor = 98.0;
        double altoFila = (height() - 2 * margen) / n;
        double altoBarra = altoFila - 8.0;
        if (altoBarra > 24.0) altoBarra = 24.0;
        double xPista = margen + anchoEtiqueta;
        double anchoPista = width() - 2 * margen - anchoEtiqueta - anchoValor;
        if (anchoPista < 40.0) anchoPista = 40.0;

        int lider = 0;
        for (int i = 1; i < n; i++)
        {
            if (mostrado[i] > mostrado[lider])
            {
                lider = i;
            }
        }

        QFont fuente = p.font();
        fuente.setPixelSize(13);
        fuente.setBold(true);
        p.setFont(fuente);

        for (int i = 0; i < n; i++)
        {
            Taxi *taxi = sim->getTaxi(i);
            double cy = margen + altoFila * (i + 0.5);
            QColor c = taxi->getColor();

            p.setPen(Qt::NoPen);
            p.setBrush(c);
            p.drawEllipse(QPointF(margen + 6, cy), 5.5, 5.5);
            p.setPen(QColor("#dce3ee"));
            p.drawText(QRectF(margen + 18, cy - 11, anchoEtiqueta - 18, 22),
                       Qt::AlignVCenter | Qt::AlignLeft, taxi->getNombre());

            QRectF pista(xPista, cy - altoBarra / 2, anchoPista, altoBarra);
            p.setPen(i == lider && mostrado[i] > 0 ? QPen(QColor("#ffd60a"), 1.5) : Qt::NoPen);
            p.setBrush(QColor("#1a2434"));
            p.drawRoundedRect(pista, 7, 7);

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
                p.drawRoundedRect(relleno, 7, 7);
            }

            p.setPen(i == lider && mostrado[i] > 0 ? QColor("#ffd60a") : QColor("#f4f7fb"));
            p.drawText(QRectF(width() - margen - anchoValor, cy - 11, anchoValor, 22),
                       Qt::AlignVCenter | Qt::AlignRight, Grafo::formatearPesos(mostrado[i]));
        }
    }

public:
    explicit BarrasGananciasWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        sim = nullptr;
        for (int i = 0; i < 7; i++)
        {
            mostrado[i] = 0.0;
        }
        escala = 50000.0;
        setMinimumHeight(250);
    }

    void setSimulacion(Simulacion *nuevaSim)
    {
        sim = nuevaSim;
        connect(sim, &Simulacion::cuadroActualizado, this, &BarrasGananciasWidget::enCuadro);
        connect(sim, &Simulacion::reinicioCompletado, this, &BarrasGananciasWidget::reiniciarBarras);
    }
};

#ifdef COMPILAR_MOCS
#include "BarrasGananciasWidget.moc"
#endif
