#ifndef MAPA_WIDGET_H
#define MAPA_WIDGET_H

#include <QColor>
#include <QFont>
#include <QPointF>
#include <QString>
#include <QTransform>
#include <QVariantAnimation>
#include <QVector>
#include <QWidget>

#include "TextoFlotante.h"

class QPainter;
class Simulacion;
class Taxi;

class MapaWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    QVariantAnimation animacionNodo;
    double oscuridad;
    int nodoBajoMouse;
    int nodoPendiente;
    bool modoConducir;
    bool mostrarPesos;
    QVector<TextoFlotante> textosFlotantes;

    static QFont fuentePixel(int px, bool negrita = false);

    static QColor obtenerColorTipoNodo(QString tipo);

    static QColor mezclarColores(QColor a, QColor b, double t);

    static void dibujarTextoConBorde(QPainter &p, QPointF centro, QString texto,
                                     QFont fuente, QColor relleno, double grosorBorde = 3.5);

    QTransform transformacionVista() const;

    QPointF posicionDibujoTaxi(const Taxi *taxi) const;

    void dibujarMundo(QPainter &p, bool esMinimapa);

    void dibujarEscenario(QPainter &p);

    void dibujarCalles(QPainter &p, bool esMinimapa);

    void dibujarRutas(QPainter &p);

    void dibujarPesos(QPainter &p);

    void dibujarNodos(QPainter &p, bool esMinimapa);

    void dibujarPasajeros(QPainter &p, bool esMinimapa);

    void dibujarTaxis(QPainter &p, bool esMinimapa);

    void dibujarEfectosNocturnos(QPainter &p);


    void dibujarMinimapa(QPainter &p);
    void seleccionarNodoEnFoco(int id);

private slots:
    void enCuadro(double dt);

    void enCarreraCompletada(int, QPointF pos, double tarifa, QColor color);

protected:
    void paintEvent(QPaintEvent *) override;

    void mousePressEvent(QMouseEvent *e) override;

    void mouseMoveEvent(QMouseEvent *e) override;

    void leaveEvent(QEvent *) override;

public:
    explicit MapaWidget(QWidget *parent = nullptr);

    void setSimulacion(Simulacion *nuevaSim);

    void setNodoPendiente(int idNodo);
    void setModoConducir(bool activo);
    void setMostrarPesos(bool mostrar);

signals:
    void nodoClicado(int idNodo);
    void nodoEnFocoCambiado(int idNodo);
    void espacioVacioClicado(QPointF posMapa);
    void cancelarSolicitado();
};

#endif // MAPA_WIDGET_H
