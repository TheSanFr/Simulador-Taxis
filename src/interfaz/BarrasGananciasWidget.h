#ifndef BARRAS_GANANCIAS_WIDGET_H
#define BARRAS_GANANCIAS_WIDGET_H

#include <QWidget>

class Simulacion;

class BarrasGananciasWidget : public QWidget
{
private:
    Simulacion *sim;
    double mostrado[7];
    double escala;

public:
    explicit BarrasGananciasWidget(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *) override;
    void showEvent(QShowEvent *event) override;

    void setSimulacion(Simulacion *nuevaSim);
    void reiniciarBarras();
    void enCuadro(double dt);
};

#endif
