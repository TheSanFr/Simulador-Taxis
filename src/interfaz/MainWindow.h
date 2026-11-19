#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QColor>
#include <QHash>
#include <QMainWindow>
#include <QPointF>
#include <QRect>
#include <QString>
#include <QTimer>
#include <QVariantAnimation>
#include <QVector>

class BarrasGananciasWidget;
class GraficoGananciasWidget;
class MapaWidget;
class QGraphicsOpacityEffect;
class QLabel;
class QListWidgetItem;
class QShortcut;
class QTableWidget;
class QWidget;
class Simulacion;
class Taxi;

namespace Ui
{
    class MainWindow;
}

class EventoRegistro
{
private:
    QString hora;
    QString categoria;
    QString titulo;
    QString detalle;
    int idTaxi;
    int idNodo;
    int identidadNodo;

public:
    EventoRegistro();
    EventoRegistro(QString hora, QString categoria, QString titulo, QString detalle,
                   int idTaxi, int idNodo, int identidadNodo);

    QString getHora() const;
    QString getCategoria() const;
    QString getTitulo() const;
    QString getDetalle() const;
    int getIdTaxi() const;
    int getIdNodo() const;
    int getIdentidadNodo() const;
};

class MainWindow : public QMainWindow
{
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
    QTimer temporizadorResalteIngreso;
    QVector<QShortcut *> atajosConfirmar;
    QString avisoTrayecto;
    bool llegadaManualReciente = false;
    int revisionPesosCotizados = 0;
    QString periodoCotizado;
    QHash<int, QString> tarifasVisibles;
    QVector<int> pasajerosEnTabla;
    int pasajeroUbicado = -1;
    QVector<EventoRegistro> eventos;

    QWidget *pestanaAnterior = nullptr;
    QLabel *capaTransicionPestana = nullptr;
    QGraphicsOpacityEffect *efectoOpacidadPestana = nullptr;
    QVariantAnimation animacionPestana;
    QRect geometriaBasePestana;
    int indicePestanaAnterior = 0;
    int desplazamientoPestana = 0;

    void aplicarSombraFlotante(QWidget *widget, int desenfoque = 11,
                               int desplazamientoY = 2, int opacidad = 100);

    const Taxi *taxiObservado() const;
    const Taxi *taxiConAviso() const;
    bool observandoTaxiAjeno() const;
    void asignarCelda(QTableWidget *tabla, int fila, int col, QString texto,
                      QColor color, Qt::Alignment alineacion);

    void actualizarTextoAyudaEdicion();
    void limpiarSeleccionEdicion();
    void confirmarCalle();
    bool tramoEnUso(int origen, int destino) const;
    QString avisoCalleOcupada() const;
    QString motivoNodoEnUso(int idNodo) const;
    bool esRutaAlcanzable(int inicio, int fin, bool excluir,
                          int nodoExcluido, int extremoA, int extremoB) const;
    bool seConservaRuta(int inicio, int fin,
                        int nodoExcluido, int extremoA, int extremoB) const;
    bool conservarViajesAlEliminar(int nodo, int extremoA, int extremoB) const;

    void mostrarMensajeEstado(QString texto);
    void registrarCambioMapa(QString titulo, QString detalle, int idNodo);
    bool mostrarEvento(const EventoRegistro &evento) const;
    void insertarEvento(const EventoRegistro &evento);
    void actualizarEventos();

    void configurarPanelGuia();
    void actualizarResumenGrafo();
    void actualizarEstadoSimulacion();
    bool pasajeroSigueEnEspera(int idPasajero) const;
    void actualizarEspera();
    void ubicarPasajero(int fila);
    void limpiarUbicacionPasajero();

    void enPausaCambiada(bool pausada);
    void enMostrarPesosCambiado(bool activo);
    void enVelocidadCambiada(int indice);
    void enModoTiempoCambiado(int indice);
    void enModoEdicionCambiado(int indice);
    void enModoConducirClicado();
    void enModoEditarClicado();
    void enHerramientaEdicionCambiada(int indice);
    void enSentidoCalleCambiado(int indice);
    void enAtajoConfirmarCalle();
    void enFilaTaxiSeleccionada(int fila, int columna);
    void enTaxiMapaCambiado(int indice);
    void enEventoListaSeleccionado(QListWidgetItem *item);
    void enPestanaCambiada(int indice);
    void enPasoAnimacionPestana();
    void enFinAnimacionPestana();
    void restaurarEstiloIngresoManual();
    void enReiniciarClicado();
    void configurarTablas();
    void actualizarTablas();

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void actualizarBotonPausa(bool pausada);
    void enCancelarClicado();
    void enEspacioVacioClicado(QPointF posMapa);
    void enNodoClicado(int idNodo);
    void enNodoEnFocoCambiado(int idNodo);
    void resaltarIngresoManual();
    void agregarEvento(QString hora, QString categoria, QString titulo,
                       QString detalle, int idTaxi, int idNodo);
    void notificarCuadroActualizado(double dtReal);
    void notificarMuestraGrafico(double segundosSimulados);
    void notificarPasajeroRecogido(int idTaxi, int origen, int destino);
    void notificarCarreraCompletada(int idTaxi, QPointF posMapa, double tarifa, QColor color);
    void notificarReinicioCompletado();
};

#endif
