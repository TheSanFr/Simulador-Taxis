#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QColor>
#include <QHash>
#include <QMainWindow>
#include <QPointF>
#include <QString>
#include <QTimer>
#include <QVector>

class BarrasGananciasWidget;
class GraficoGananciasWidget;
class MapaWidget;
class QShortcut;
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

    int modoEdicion;
    int nodoSeleccionadoEdicion;
    int destinoEdicion = -1;
    bool pausaAntesDeEditar = false;
    QTimer temporizadorTablas;
    QTimer temporizadorAviso;
    QVector<QShortcut *> atajosConfirmar;
    QString avisoTrayecto;
    bool llegadaManualReciente = false;
    quint64 revisionPesosCotizados = 0;
    QString periodoCotizado;
    QHash<int, QString> tarifasVisibles;
    QVector<int> pasajerosEnTabla;
    int pasajeroUbicado = -1;

    struct Evento
    {
        QString hora;
        QString categoria;
        QString titulo;
        QString detalle;
        int idTaxi;
        int idNodo;
        quint64 identidadNodo;
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
    QString avisoCalleOcupada() const;
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
    void actualizarEspera();
    void ubicarPasajero(int fila);
    void limpiarUbicacionPasajero();

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

#endif
