#ifndef GRAFICO_GANANCIAS_WIDGET_H
#define GRAFICO_GANANCIAS_WIDGET_H

#include <QWidget>

class QChart;
class QChartView;
class QLineSeries;
class QValueAxis;
class Simulacion;

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
    void reconstruirSeries();

    void enMuestra(double segundos);

public:
    explicit GraficoGananciasWidget(QWidget *parent = nullptr);

    void setSimulacion(Simulacion *nuevaSim);
};

#endif
