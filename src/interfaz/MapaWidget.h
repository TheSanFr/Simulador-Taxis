#ifndef MAPA_WIDGET_H
#define MAPA_WIDGET_H

#include <QColor>
#include <QFont>
#include <QFontMetricsF>
#include <QLineF>
#include <QPainterPath>
#include <QPixmap>
#include <QPointF>
#include <QRectF>
#include <QStaticText>
#include <QString>
#include <QTimer>
#include <QTransform>
#include <QVariantAnimation>
#include <QVector>
#include <QWidget>

#include "interfaz/TextoFlotante.h"

class MainWindow;
class QPainter;
class Simulacion;
class Taxi;

class TramoVisible
{
private:
    QLineF eje;
    QLineF lineaCentral;
    QPointF flechaIda[3];
    QPointF flechaVuelta[3];
    bool visible;
    bool avenida;
    bool dobleSentido;
    bool dibujarFlechas;

public:
    TramoVisible();

    QLineF getEje() const;
    void setEje(QLineF nuevoEje);

    QLineF getLineaCentral() const;
    void setLineaCentral(QLineF nuevaLinea);

    const QPointF *getFlechaIda() const;
    QPointF *datosFlechaIda();

    const QPointF *getFlechaVuelta() const;
    QPointF *datosFlechaVuelta();

    bool esVisible() const;
    void setVisible(bool valor);

    bool esAvenida() const;
    void setAvenida(bool valor);

    bool esDobleSentido() const;
    void setDobleSentido(bool valor);

    bool debeDibujarFlechas() const;
    void setDibujarFlechas(bool valor);
};

class NombreNodo
{
private:
    int id;
    QPointF posicion;
    QStaticText texto;
    QColor color;
    bool especial;

public:
    NombreNodo();
    NombreNodo(int id, QPointF posicion, QStaticText texto, QColor color, bool especial);

    int getId() const;
    QPointF getPosicion() const;
    const QStaticText &getTexto() const;
    QColor getColor() const;
    bool esEspecial() const;
};

class MapaWidget : public QWidget
{
private:
    Simulacion *sim;
    MainWindow *ventana;
    QVariantAnimation animacionNodo;
    QVariantAnimation animacionConfirmacion;
    double oscuridad;
    int nodoBajoMouse;
    QPointF posicionCursorMapa;
    int nodoPendiente;
    int herramientaEdicion = 0;
    int destinoProvisional = -1;
    bool calleDobleSentido = true;
    bool cursorEnMapa = false;
    QPointF inicioConfirmacion;
    QPointF finConfirmacion;
    bool confirmarTramo = false;
    QVector<TramoVisible> tramosPorArista;
    QRectF encuadreConduccion;
    bool geometriaPendiente = true;
    int ultimaRevisionGrafo = 0;
    QPainterPath corredorCalles;
    QPainterPath terrenoDisponible;
    QPainterPath plataformaPuentes;
    bool corredorPendiente = true;
    QPixmap fondoCiudad;
    QVector<NombreNodo> nombresNodos;
    double escalaNombres = 0.0;
    bool modoConducir;
    bool mostrarPesos;
    int taxiEnFoco = -1;
    QVector<TextoFlotante> textosFlotantes;
    bool animCobroActiva = false;
    QPointF animCobroInicio;
    QPointF animCobroFin;
    double animCobroProgreso = 0.0;
    double animCobroDuracion = 0.55;
    QString animCobroTexto;
    double tiempoPausaCobro = 0.0;
    double tarifaCobradaReciente = 0.0;
    QTimer temporizadorRecorrido;
    QPointF recogidaConsultada;
    QPointF destinoConsultado;

    QFont fuentePixel(int px, bool negrita = false) const;

    QColor obtenerColorTipoNodo(QString tipo) const;

    QColor mezclarColores(QColor a, QColor b, double t) const;

    void dibujarTextoConBorde(QPainter &p, QPointF centro, QString texto,
                              QFont fuente, QColor relleno, double grosorBorde = 3.5) const;

    QRectF areaMarcoMapa() const;
    QTransform transformacionVista();
    void sincronizarRevisionGrafo();
    void actualizarGeometria();
    void invalidarGeometria();
    int nodoInteractivo(QPointF posMapa) const;

    QPointF posicionDibujoTaxi(const Taxi *taxi) const;
    bool mostrarRutaDelTaxi(const Taxi *taxi) const;

    void dibujarTechoManzana(QPainter &p, QRectF volumen, QColor colorTecho) const;
    bool ubicarHitoManzana(const QPainterPath &recorte, QPointF centro,
                           const QPainterPath &huella, QPointF &posicionElegida) const;
    void dibujarRotuloEdicion(QPainter &p, QPointF referencia, QString texto, QColor acento) const;
    void dibujarMarcaRecorrido(QPainter &p, QRectF limites, QFontMetricsF medidas,
                               QPointF cruce, QString texto, QColor color, bool recogida) const;

    void dibujarMundo(QPainter &p, bool esMinimapa);

    void dibujarEscenario(QPainter &p);
    void dibujarFondoCiudad(QPainter &p, bool esMinimapa);
    void dibujarNombresNodos(QPainter &p, int destinoVisible);
    void actualizarCorredorCalles();
    void dibujarVistaPreviaEdicion(QPainter &p);
    void dibujarConfirmacion(QPainter &p);
    void dibujarRecorridoConsultado(QPainter &p);

    void dibujarCalles(QPainter &p, bool esMinimapa);

    void dibujarRutas(QPainter &p);

    void dibujarPesos(QPainter &p);

    void dibujarNodos(QPainter &p, bool esMinimapa);

    void dibujarPasajeros(QPainter &p, bool esMinimapa);

    void dibujarTaxis(QPainter &p, bool esMinimapa);

    void dibujarEfectosNocturnos(QPainter &p);

    QRectF areaMinimapa() const;
    void dibujarMinimapa(QPainter &p);
    QRectF areaTaximetro() const;
    void dibujarTaximetro(QPainter &p);
    void dibujarAnimacionCobro(QPainter &p);
    void dibujarMarcoEsquinas(QPainter &p, QRectF marco) const;
    void seleccionarNodoEnFoco(int id);
    void actualizarVista();

public:
    explicit MapaWidget(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *) override;

    void mousePressEvent(QMouseEvent *e) override;

    void mouseMoveEvent(QMouseEvent *e) override;

    void leaveEvent(QEvent *) override;

    void setSimulacion(Simulacion *nuevaSim);
    void setVentanaPrincipal(MainWindow *nuevaVentana);

    void enCuadro(double dt);
    void enCarreraCompletada(int idTaxi, QPointF pos, double tarifa, QColor color);
    void enPasajeroRecogido(int idTaxi, int origen, int destino);
    void enReinicioSimulacion();

    void setNodoPendiente(int idNodo);
    void setHerramientaEdicion(int modo);
    void setSentidoCalle(bool doble);
    void setDestinoProvisional(int idDestino);
    void resaltarNodo(int idNodo);
    void resaltarRecorridoPasajero(int origen, int destino);
    void ocultarRecorridoPasajero();
    void resaltarCalle(int origen, int destino);
    void setModoConducir(bool activo);
    void setMostrarPesos(bool mostrar);
    void setTaxiEnFoco(int idTaxi);
};

#endif
