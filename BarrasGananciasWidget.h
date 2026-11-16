#ifndef BARRAS_GANANCIAS_WIDGET_H
#define BARRAS_GANANCIAS_WIDGET_H

#include <QWidget>

class Simulacion;

class BarrasGananciasWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    double mostrado[7];
    double escala;

private slots:
    void reiniciarBarras();

    void enCuadro(double dt);

protected:
    void paintEvent(QPaintEvent *) override;

public:
    explicit BarrasGananciasWidget(QWidget *parent = nullptr);

    void setSimulacion(Simulacion *nuevaSim);
};

#endif // BARRAS_GANANCIAS_WIDGET_H
