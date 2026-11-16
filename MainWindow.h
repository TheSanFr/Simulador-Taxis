#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QColor>
#include <QMainWindow>
#include <QPointF>
#include <QString>
#include <QTimer>
#include <QVector>

class BarrasGananciasWidget;
class GraficoGananciasWidget;
class MapaWidget;
class QTableWidget;
class Simulacion;
class Taxi;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    Simulacion *sim;
    MapaWidget *mapaWidget;
    BarrasGananciasWidget *barrasWidget;
    GraficoGananciasWidget *graficoWidget;

    /*
        Modos de edición:
        0 = conducir taxi manual
        1 = agregar nodo
        2 = agregar calle
        3 = eliminar calle
        4 = eliminar nodo
        5 = cambiar peso de calle
    */
    int modoEdicion;
    int nodoSeleccionadoEdicion;
    int destinoEdicion = -1;
    bool pausaAntesDeEditar = false;
    QTimer temporizadorTablas;
    QTimer temporizadorAviso;
    QString avisoTrayecto;
    bool llegadaManualReciente = false;

    struct Evento
    {
        QString hora;
        QString categoria;
        QString titulo;
        QString detalle;
        int idTaxi;
        int idNodo;
    };
    QVector<Evento> eventos;

    const Taxi *taxiObservado() const;
    const Taxi *taxiConAviso() const;
    bool observandoTaxiAjeno() const;
    static void asignarCelda(QTableWidget *tabla, int fila, int col, QString texto,
                             QColor color, Qt::Alignment alineacion);

    void actualizarTextoAyudaEdicion();
    void limpiarSeleccionEdicion();
    void confirmarCalle();
    bool tramoEnUso(int origen, int destino) const;
    QString motivoNodoEnUso(int idNodo) const;
    bool conservarViajesAlEliminar(int nodo, int extremoA, int extremoB) const;

    void mostrarMensajeEstado(QString texto);
    void registrarCambioMapa(QString titulo, QString detalle, int idNodo);
    bool mostrarEvento(const Evento &evento) const;
    void insertarEvento(const Evento &evento);
    void actualizarEventos();

    void configurarPanelGuia();
    void actualizarResumenGrafo();
    void actualizarEstadoSimulacion();

private slots:
    void enPausaCambiada(bool pausada);

    void actualizarBotonPausa(bool pausada);

    void enMostrarPesosCambiado(bool activo);

    void enVelocidadCambiada(int indice);

    void enModoTiempoCambiado(int indice);

    void enModoEdicionCambiado(int indice);

    void enReiniciarClicado();

    void enCancelarClicado();

    void enEspacioVacioClicado(QPointF posMapa);

    void enNodoClicado(int idNodo);

    void agregarEvento(QString hora, QString categoria, QString titulo, QString detalle, int idTaxi, int idNodo);

    void configurarTablas();

    void actualizarTablas();

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;
};

#endif // MAIN_WINDOW_H
