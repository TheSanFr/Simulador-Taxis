#include "interfaz/MainWindow.h"

#include "modelo/Arista.h"
#include "interfaz/BarrasGananciasWidget.h"
#include "interfaz/DialogoNodo.h"
#include "estructuras/ElementoArista.h"
#include "interfaz/GraficoGananciasWidget.h"
#include "simulacion/Grafo.h"
#include "estructuras/ListaAristas.h"
#include "estructuras/ListaNodos.h"
#include "interfaz/MapaWidget.h"
#include "modelo/Nodo.h"
#include "modelo/Pasajero.h"
#include "simulacion/RelojSimulacion.h"
#include "simulacion/Simulacion.h"
#include "taxis/Taxi.h"
#include "taxis/TaxiManual.h"
#include "ui_mainwindow.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QEasingCurve>
#include <QFontMetrics>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QHeaderView>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QScrollBar>
#include <QSet>
#include <QShortcut>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QTextCursor>
#include <QVector>
#include <QWidget>

EventoRegistro::EventoRegistro()
    : idTaxi(-1), idNodo(-1), identidadNodo(0)
{
}

EventoRegistro::EventoRegistro(QString hora, QString categoria, QString titulo, QString detalle,
                               int idTaxi, int idNodo, int identidadNodo)
    : hora(hora), categoria(categoria), titulo(titulo), detalle(detalle),
      idTaxi(idTaxi), idNodo(idNodo), identidadNodo(identidadNodo)
{
}

QString EventoRegistro::getHora() const
{
    return hora;
}

QString EventoRegistro::getCategoria() const
{
    return categoria;
}

QString EventoRegistro::getTitulo() const
{
    return titulo;
}

QString EventoRegistro::getDetalle() const
{
    return detalle;
}

int EventoRegistro::getIdTaxi() const
{
    return idTaxi;
}

int EventoRegistro::getIdNodo() const
{
    return idNodo;
}

int EventoRegistro::getIdentidadNodo() const
{
    return identidadNodo;
}

void MainWindow::aplicarSombraFlotante(QWidget *widget, int desenfoque,
                                       int desplazamientoY, int opacidad)
{
    if (widget == nullptr)
    {
        return;
    }
    QGraphicsDropShadowEffect *sombra = new QGraphicsDropShadowEffect(widget);
    sombra->setBlurRadius(desenfoque);
    sombra->setOffset(0, desplazamientoY);
    sombra->setColor(QColor(0, 0, 0, opacidad));
    widget->setGraphicsEffect(sombra);
}

void MainWindow::asignarCelda(QTableWidget *tabla, int fila, int col, QString texto,
                              QColor color, Qt::Alignment alineacion)
{
    QTableWidgetItem *item = tabla->item(fila, col);
    if (item == nullptr)
    {
        item = new QTableWidgetItem();
        tabla->setItem(fila, col, item);
    }
    if (item->text() != texto)
        item->setText(texto);
    if (item->foreground().color() != color || item->foreground().style() == Qt::NoBrush)
        item->setForeground(color);
    if (Qt::Alignment(item->textAlignment()) != (alineacion | Qt::AlignVCenter))
        item->setTextAlignment(alineacion | Qt::AlignVCenter);
}

const Taxi *MainWindow::taxiObservado() const
{
    const int id = ui->comboMapTaxi->currentData().toInt();
    for (int i = 0; i < sim->getCantidadTaxis(); ++i)
    {
        const Taxi *taxi = sim->getTaxi(i);
        if (taxi->getId() == id)
            return taxi;
    }
    return nullptr;
}

bool MainWindow::observandoTaxiAjeno() const
{
    const Taxi *taxi = taxiObservado();
    return taxi != nullptr && !taxi->esManual();
}

const Taxi *MainWindow::taxiConAviso() const
{
    if (const Taxi *taxi = taxiObservado())
        return taxi;
    return sim->getTaxiManual();
}

void MainWindow::actualizarTextoAyudaEdicion()
{
    QString texto;
    if (modoEdicion == 0)
    {
        const Taxi *taxi = taxiObservado();
        if (observandoTaxiAjeno())
            texto = QStringLiteral("%1 · %2").arg(taxi->getNombre(), taxi->textoEstado(sim->getGrafo()));
        else if (sim->getTaxiManual()->getLlevaPasajero())
            texto = QStringLiteral("Avanza hacia la bandera de destino.");
        else
            texto = QStringLiteral("Avanza por los cruces resaltados hacia un cliente.");

        if (temporizadorAviso.isActive())
            texto = avisoTrayecto;
    }
    else if (modoEdicion == 1)
        texto = QStringLiteral("Pulsa el mapa para colocar un nodo o crear un cruce sobre una calle.");
    else if (modoEdicion == 2)
    {
        bool puedeConstruir = false;
        if (nodoSeleccionadoEdicion < 0)
            texto = QStringLiteral("Selecciona el cruce de origen.");
        else if (destinoEdicion < 0)
            texto = QStringLiteral("Señala un destino para ver la calle. Haz clic para fijarlo.");
        else
        {
            const RevisionCalle revision = sim->getGrafo()->previsualizarCalle(
                nodoSeleccionadoEdicion, destinoEdicion, ui->comboSentidoCalle->currentIndex() == 0);
            if (!revision.esValida())
                texto = revision.getMotivo();
            else
            {
                puedeConstruir = true;
                texto = revision.getCruces().isEmpty()
                    ? QStringLiteral("Peso inicial %1 · Enter para construir.").arg(revision.getPeso(), 0, 'f', 1)
                    : QStringLiteral("Peso inicial %1 · cruces intermedios: %2 · Enter para construir.")
                        .arg(revision.getPeso(), 0, 'f', 1).arg(revision.getCruces().size());
            }
            const QVector<QPair<int, int>> &tramos = revision.getTramosAfectados();
            for (int i = 0; i < tramos.size(); ++i)
            {
                if (revision.esValida() && tramoEnUso(tramos[i].first, tramos[i].second))
                {
                    texto = avisoCalleOcupada();
                    puedeConstruir = false;
                    break;
                }
            }
        }
        ui->btnConstruirCalle->setEnabled(puedeConstruir);
    }
    else if (modoEdicion == 3)
        texto = QStringLiteral("Selecciona los dos extremos de la calle.");
    else if (modoEdicion == 4)
        texto = QStringLiteral("Selecciona el nodo que quieres eliminar junto con sus calles.");
    else if (modoEdicion == 5)
        texto = QStringLiteral("Selecciona los dos extremos de la calle para cambiar su peso.");
    for (int i = 0; i < atajosConfirmar.size(); ++i)
        atajosConfirmar[i]->setEnabled(modoEdicion == 2 && ui->btnConstruirCalle->isEnabled());
    const int anchoDisponible = ui->lblEditHint->width() > 40 ? ui->lblEditHint->width() : 520;
    ui->lblEditHint->setText(ui->lblEditHint->fontMetrics().elidedText(texto, Qt::ElideRight, anchoDisponible));
    ui->lblEditHint->setToolTip(texto);
}

void MainWindow::mostrarMensajeEstado(QString texto)
{
    ui->statusbar->showMessage(texto, 8000);
    const int anchoDisponible = ui->lblEditHint->width() > 40 ? ui->lblEditHint->width() : 520;
    ui->lblEditHint->setText(ui->lblEditHint->fontMetrics().elidedText(texto, Qt::ElideRight, anchoDisponible));
    ui->lblEditHint->setToolTip(texto);
}

void MainWindow::configurarPanelGuia()
{
    ui->txtInfo->setHtml(QStringLiteral(R"(
<style>
h2 { color: #fff0cc; margin-top: 16px; margin-bottom: 5px; }
p, li { color: #d9e4df; }
</style>
<h2>Tu primera carrera</h2>
<ol>
<li>En <b>Conducir</b>, busca a una persona turquesa. Señala un cruce resaltado para previsualizar el siguiente tramo y pulsa para avanzar.</li>
<li>Al llegar, el primer taxi vacío recoge al pasajero. El panel cambia a <b>A bordo</b> y aparece una bandera.</li>
<li>Sigue los cruces permitidos hasta la bandera. El mapa confirma la llegada y el panel registra el ingreso.</li>
</ol>
<p>Usa clic derecho para cancelar los tramos pendientes. En <b>Rutas</b> puedes destacar otro taxi sin dejar de conducir.</p>
<h2>Editar la ciudad</h2>
<p>En <b>Editar mapa</b>, la simulación se pone en pausa. Para crear una calle, elige el origen,
señala el destino y revisa las flechas. Fija el destino con un clic. Elige el sentido y pulsa <b>Construir</b> o <b>Enter</b>.
Usa <b>Esc</b> para cancelar.</p>
<p>Si la calle atraviesa otra vía, la vista previa marca dónde se unirán.
Con <b>Agregar nodo</b>, puedes colocar un punto libre o dividir una calle. Volver a <b>Conducir</b> restaura la pausa anterior.</p>
<h2>Comparar taxis</h2>
<p>Hay seis taxis autónomos y uno manual. <b>BFS</b> busca una ruta con menos saltos,
<b>DFS</b> explora una rama a fondo y <b>Dijkstra</b> busca una ruta de menor peso.
Selecciona una fila de la flota para ver ese recorrido en el mapa.</p>
<h2>Tarifas y tráfico</h2>
<ul>
<li>La ciudad es un grafo dirigido y ponderado. Las flechas indican el sentido y el peso representa el costo del tramo.</li>
<li>El taxímetro avanza con los saltos y el tiempo ponderado recorridos durante la carrera. Se calcula con base de $4.600, $1.625 por salto y
$1.625 por unidad de tiempo ponderado, con un mínimo de $8.100 antes de recargos.</li>
<li>Aeropuerto: +$8.600. Terminal: +$2.400. Turismo: +$2.000 y mayor probabilidad de ser destino.</li>
<li>El uso eleva el peso de la calle. En hora pico el peso efectivo se multiplica por 1,8.
El período nocturno añade $2.000 a la tarifa.</li>
</ul>
)"));
    ui->txtInfo->moveCursor(QTextCursor::Start);
}

void MainWindow::enPausaCambiada(bool pausada)
{
    sim->setPausada(pausada);
}

void MainWindow::actualizarBotonPausa(bool pausada)
{
    ui->btnPause->blockSignals(true);
    ui->btnPause->setChecked(pausada);
    ui->btnPause->setText(pausada ? QStringLiteral("Reanudar") : QStringLiteral("Pausar"));
    ui->btnPause->blockSignals(false);
}

void MainWindow::enMostrarPesosCambiado(bool activo)
{
    mapaWidget->setMostrarPesos(activo);
}

void MainWindow::enVelocidadCambiada(int indice)
{
    if (indice == 1) sim->setVelocidad(2.0);
    else if (indice == 2) sim->setVelocidad(4.0);
    else sim->setVelocidad(1.0);
}

void MainWindow::enModoTiempoCambiado(int indice)
{
    sim->setModoTiempo(indice);
}

void MainWindow::enModoEdicionCambiado(int indice)
{
    const bool entrando = modoEdicion == 0 && indice != 0;
    const bool saliendo = modoEdicion != 0 && indice == 0;
    if (entrando)
    {
        pausaAntesDeEditar = sim->estaPausada();
        sim->setPausada(true);
    }
    else if (saliendo)
    {
        sim->setPausada(pausaAntesDeEditar);
    }
    modoEdicion = indice;
    limpiarSeleccionEdicion();
    mapaWidget->setModoConducir(modoEdicion == 0);
    mapaWidget->setHerramientaEdicion(modoEdicion);
    ui->comboEditMode->setVisible(modoEdicion != 0);
    ui->comboSentidoCalle->setVisible(modoEdicion == 2);
    ui->btnConstruirCalle->setVisible(modoEdicion == 2);
    ui->btnPause->setEnabled(modoEdicion == 0);
    ui->btnPause->setToolTip(modoEdicion == 0 ? QString() : QStringLiteral("La ciudad está en pausa durante la edición."));
    ui->lblMapTaxi->setVisible(modoEdicion == 0);
    ui->comboMapTaxi->setVisible(modoEdicion == 0);
    actualizarTextoAyudaEdicion();
}

void MainWindow::enModoConducirClicado()
{
    enModoEdicionCambiado(0);
}

void MainWindow::enModoEditarClicado()
{
    enModoEdicionCambiado(ui->comboEditMode->currentIndex() + 1);
}

void MainWindow::enHerramientaEdicionCambiada(int indice)
{
    if (ui->btnEditMode->isChecked())
        enModoEdicionCambiado(indice + 1);
}

void MainWindow::enSentidoCalleCambiado(int indice)
{
    mapaWidget->setSentidoCalle(indice == 0);
    actualizarTextoAyudaEdicion();
}

void MainWindow::enAtajoConfirmarCalle()
{
    if (modoEdicion == 2 && ui->btnConstruirCalle->isEnabled())
        confirmarCalle();
}

void MainWindow::enFilaTaxiSeleccionada(int fila, int)
{
    if (fila < 0 || fila >= sim->getCantidadTaxis())
        return;
    const Taxi *taxi = sim->getTaxi(fila);
    const int indice = ui->comboMapTaxi->findData(taxi->getId());
    if (indice >= 0)
        ui->comboMapTaxi->setCurrentIndex(indice);
}

void MainWindow::enTaxiMapaCambiado(int)
{
    temporizadorAviso.stop();
    mapaWidget->setTaxiEnFoco(ui->comboMapTaxi->currentData().toInt());
    actualizarEstadoSimulacion();
}

void MainWindow::enEventoListaSeleccionado(QListWidgetItem *item)
{
    if (item == nullptr)
        return;
    const int id = item->data(Qt::UserRole).toInt();
    const int identidad = item->data(Qt::UserRole + 1).toInt();
    if (identidad != 0 && identidad == sim->getGrafo()->getIdentidadNodo(id))
        mapaWidget->resaltarNodo(id);
    else
        item->setToolTip(item->text());
}

void MainWindow::enPestanaCambiada(int indice)
{
    if (ui->tabs->currentWidget() != ui->tabPassengers)
        limpiarUbicacionPasajero();
    actualizarTablas();

    QWidget *saliente = pestanaAnterior;
    QWidget *entrante = ui->tabs->widget(indice);
    int direccion = (indice >= indicePestanaAnterior) ? -1 : 1;
    indicePestanaAnterior = indice;
    pestanaAnterior = entrante;

    if (capaTransicionPestana == nullptr || efectoOpacidadPestana == nullptr)
        return;

    animacionPestana.stop();
    capaTransicionPestana->hide();

    if (saliente != nullptr && entrante != nullptr && entrante->isVisible())
    {
        QPixmap captura = saliente->grab();
        if (!captura.isNull())
        {
            geometriaBasePestana = entrante->geometry();
            desplazamientoPestana = direccion * 14;
            capaTransicionPestana->setPixmap(captura);
            capaTransicionPestana->setGeometry(geometriaBasePestana);
            efectoOpacidadPestana->setOpacity(1.0);
            capaTransicionPestana->raise();
            capaTransicionPestana->show();
            animacionPestana.start();
        }
    }
}

void MainWindow::enPasoAnimacionPestana()
{
    if (capaTransicionPestana == nullptr || efectoOpacidadPestana == nullptr)
        return;
    const double avance = animacionPestana.currentValue().toDouble();
    efectoOpacidadPestana->setOpacity(1.0 - avance);
    const int dx = qRound(desplazamientoPestana * avance);
    capaTransicionPestana->setGeometry(geometriaBasePestana.translated(dx, 0));
}

void MainWindow::enFinAnimacionPestana()
{
    if (capaTransicionPestana == nullptr)
        return;
    capaTransicionPestana->hide();
    capaTransicionPestana->clear();
}

void MainWindow::limpiarSeleccionEdicion()
{
    nodoSeleccionadoEdicion = -1;
    destinoEdicion = -1;
    mapaWidget->setNodoPendiente(-1);
    mapaWidget->setDestinoProvisional(-1);
}

void MainWindow::enReiniciarClicado()
{
    bool estabaPausada = sim->estaPausada();
    sim->setPausada(true);
    if (QMessageBox::question(this, QStringLiteral("Reiniciar"),
                              QStringLiteral("¿Deseas reiniciar los taxis, ingresos y el mapa original?"))
        == QMessageBox::Yes)
    {
        sim->reiniciar();
    }
    sim->setPausada(estabaPausada);
}

void MainWindow::enCancelarClicado()
{
    if (modoEdicion == 0 && sim->getTaxiManual() != nullptr)
    {
        sim->getTaxiManual()->limpiarRuta();
        mostrarMensajeEstado(QStringLiteral("Ruta manual cancelada."));
    }
    else
    {
        limpiarSeleccionEdicion();
        actualizarTextoAyudaEdicion();
    }
}

void MainWindow::enEspacioVacioClicado(QPointF posMapa)
{
    if (modoEdicion != 1)
    {
        return;
    }

    Grafo *grafo = sim->getGrafo();
    const RevisionNodo revision = grafo->previsualizarNodo(posMapa);
    if (!revision.esValida())
    {
        mostrarMensajeEstado(revision.getMotivo());
        return;
    }
    const QVector<QPair<int, int>> &tramos = revision.getTramosAfectados();
    for (int i = 0; i < tramos.size(); ++i)
    {
        if (tramoEnUso(tramos[i].first, tramos[i].second))
        {
            mostrarMensajeEstado(avisoCalleOcupada());
            return;
        }
    }
    DialogoNodo dialogo(this);
    if (revision.getDivideCalle())
        dialogo.setWindowTitle(QStringLiteral("Crear cruce en la calle"));
    if (dialogo.exec() == QDialog::Accepted)
    {
        QString nombre = dialogo.getNombreNodo();
        if (nombre.isEmpty())
        {
            nombre = revision.getDivideCalle() ? QStringLiteral("Nuevo cruce") : QStringLiteral("Nuevo nodo");
        }
        QString motivo;
        const int id = grafo->crearNodoEditado(posMapa, nombre, dialogo.getTipoNodo(), revision.getDivideCalle(), &motivo);
        if (id < 0)
        {
            mostrarMensajeEstado(motivo);
            return;
        }
        actualizarResumenGrafo();
        mapaWidget->resaltarNodo(id);
        const QString titulo = revision.getDivideCalle() ? QStringLiteral("Cruce creado") : QStringLiteral("Nodo creado");
        registrarCambioMapa(titulo, nombre, id);
        mostrarMensajeEstado(revision.getDivideCalle()
            ? QStringLiteral("Cruce %1 conectado a la calle.").arg(nombre)
            : QStringLiteral("%1 está sin conectar. Usa Agregar calle.").arg(nombre));
    }
}

bool MainWindow::tramoEnUso(int origen, int destino) const
{
    for (int i = 0; i < sim->getCantidadTaxis(); ++i)
    {
        const Taxi *taxi = sim->getTaxi(i);
        if (!taxi->getEnMovimiento())
            continue;
        const int desde = taxi->getNodoActual();
        const int hasta = taxi->getNodoSiguiente();
        if ((desde == origen && hasta == destino) || (desde == destino && hasta == origen))
            return true;
    }
    return false;
}

QString MainWindow::avisoCalleOcupada() const
{
    return pausaAntesDeEditar
        ? QStringLiteral("Vuelve a Conducir, pulsa Reanudar y espera a que el taxi termine de cruzar.")
        : QStringLiteral("Vuelve a Conducir y espera a que el taxi termine de cruzar.");
}

QString MainWindow::motivoNodoEnUso(int idNodo) const
{
    for (int i = 0; i < sim->getCantidadTaxis(); ++i)
    {
        const Taxi *taxi = sim->getTaxi(i);
        if (taxi->getNodoActual() == idNodo || (taxi->getEnMovimiento() && taxi->getNodoSiguiente() == idNodo))
            return QStringLiteral("Hay un taxi en ese cruce o en un tramo que lo conecta.");
        if (taxi->getLlevaPasajero() && taxi->getDestinoPasajeroABordo() == idNodo)
            return QStringLiteral("Ese cruce es el destino de un pasajero a bordo.");
    }
    for (const Pasajero &pasajero : sim->getPasajerosEnEspera())
    {
        if (pasajero.getOrigen() == idNodo || pasajero.getDestino() == idNodo)
            return QStringLiteral("Ese cruce forma parte del viaje de un pasajero en espera.");
    }
    return {};
}

bool MainWindow::esRutaAlcanzable(int inicio, int fin, bool excluir,
                                  int nodoExcluido, int extremoA, int extremoB) const
{
    const Grafo *grafo = sim->getGrafo();
    QVector<int> pendientes;
    pendientes.append(inicio);
    QSet<int> visitados;
    visitados.insert(inicio);

    for (int i = 0; i < pendientes.size(); ++i)
    {
        const int actual = pendientes[i];
        if (actual == fin)
            return true;
        for (const ElementoArista *elemento = grafo->getAristas()->getPrimero();
             elemento != nullptr; elemento = elemento->getSiguiente())
        {
            const Arista *arista = elemento->getArista();
            if (arista->getOrigen() != actual)
                continue;
            const int siguiente = arista->getDestino();
            const bool tramoExcluido = (actual == extremoA && siguiente == extremoB)
                || (actual == extremoB && siguiente == extremoA);
            if (excluir && (siguiente == nodoExcluido || tramoExcluido))
                continue;
            if (!visitados.contains(siguiente))
            {
                visitados.insert(siguiente);
                pendientes.append(siguiente);
            }
        }
    }
    return false;
}

bool MainWindow::seConservaRuta(int inicio, int fin,
                                int nodoExcluido, int extremoA, int extremoB) const
{
    return !esRutaAlcanzable(inicio, fin, false, nodoExcluido, extremoA, extremoB)
        || esRutaAlcanzable(inicio, fin, true, nodoExcluido, extremoA, extremoB);
}

bool MainWindow::conservarViajesAlEliminar(int nodo, int extremoA, int extremoB) const
{
    for (const Pasajero &pasajero : sim->getPasajerosEnEspera())
    {
        if (!seConservaRuta(pasajero.getOrigen(), pasajero.getDestino(), nodo, extremoA, extremoB))
            return false;
    }
    for (int i = 0; i < sim->getCantidadTaxis(); ++i)
    {
        const Taxi *taxi = sim->getTaxi(i);
        const int partida = taxi->getEnMovimiento() ? taxi->getNodoSiguiente() : taxi->getNodoActual();
        if (taxi->getLlevaPasajero()
            && !seConservaRuta(partida, taxi->getDestinoPasajeroABordo(), nodo, extremoA, extremoB))
            return false;
    }
    return true;
}

void MainWindow::confirmarCalle()
{
    if (modoEdicion != 2 || nodoSeleccionadoEdicion < 0 || destinoEdicion < 0)
        return;
    Grafo *grafo = sim->getGrafo();
    const int origen = nodoSeleccionadoEdicion;
    const int destino = destinoEdicion;
    const bool doble = ui->comboSentidoCalle->currentIndex() == 0;
    const RevisionCalle revision = grafo->previsualizarCalle(origen, destino, doble);
    if (!revision.esValida())
    {
        mostrarMensajeEstado(revision.getMotivo());
        return;
    }
    const QVector<QPair<int, int>> &tramos = revision.getTramosAfectados();
    for (int i = 0; i < tramos.size(); ++i)
    {
        if (tramoEnUso(tramos[i].first, tramos[i].second))
        {
            mostrarMensajeEstado(avisoCalleOcupada());
            return;
        }
    }
    const QString enlace = QStringLiteral("%1 %2 %3")
        .arg(grafo->buscarNodo(origen)->getNombre(), doble ? QStringLiteral("↔") : QStringLiteral("→"),
             grafo->buscarNodo(destino)->getNombre());
    QString motivo;
    if (!grafo->crearCalleEditada(origen, destino, doble, true, &motivo))
    {
        mostrarMensajeEstado(motivo);
        return;
    }
    actualizarResumenGrafo();
    limpiarSeleccionEdicion();
    actualizarTextoAyudaEdicion();
    mapaWidget->resaltarCalle(origen, destino);
    QString detalle = QStringLiteral("%1 · peso %2").arg(enlace).arg(revision.getPeso(), 0, 'f', 1);
    if (!revision.getCruces().isEmpty())
        detalle += QStringLiteral(" · cruces intermedios: %1").arg(revision.getCruces().size());
    registrarCambioMapa(QStringLiteral("Calle creada"), detalle, destino);
    mostrarMensajeEstado(QStringLiteral("Calle creada · %1").arg(enlace));
}

void MainWindow::enNodoClicado(int idNodo)
{
    Grafo *g = sim->getGrafo();
    Nodo *nodoClic = g->buscarNodo(idNodo);
    if (nodoClic == nullptr)
    {
        return;
    }

    if (modoEdicion == 0)
    {
        if (sim->getTaxiManual() != nullptr && !sim->getTaxiManual()->agregarNodoARuta(idNodo, g))
        {
            mostrarMensajeEstado(
                QStringLiteral("No hay una calle directa hacia \"%1\".").arg(nodoClic->getNombre()));
        }
        return;
    }

    if (modoEdicion == 1)
    {
        return;
    }

    if (modoEdicion == 4)
    {
        const QString motivo = motivoNodoEnUso(idNodo);
        if (!motivo.isEmpty())
        {
            mostrarMensajeEstado(motivo);
            return;
        }
        if (!conservarViajesAlEliminar(idNodo, -1, -1))
        {
            mostrarMensajeEstado(QStringLiteral("Eliminar este cruce dejaría un viaje sin ruta."));
            return;
        }
        QString nombre = nodoClic->getNombre();
        if (QMessageBox::question(this, QStringLiteral("Eliminar nodo"),
                                  QStringLiteral("¿Eliminar \"%1\" y sus calles?").arg(nombre))
            == QMessageBox::Yes)
        {
            if (g->eliminarNodo(idNodo))
            {
                actualizarResumenGrafo();
                registrarCambioMapa(QStringLiteral("Nodo eliminado"), nombre, -1);
                mostrarMensajeEstado(QStringLiteral("Nodo \"%1\" eliminado.").arg(nombre));
            }
        }
        return;
    }

    if (nodoSeleccionadoEdicion < 0)
    {
        ui->statusbar->clearMessage();
        nodoSeleccionadoEdicion = idNodo;
        mapaWidget->setNodoPendiente(idNodo);
        actualizarTextoAyudaEdicion();
        return;
    }

    if (modoEdicion == 2)
    {
        destinoEdicion = idNodo;
        mapaWidget->setDestinoProvisional(idNodo);
        ui->statusbar->clearMessage();
        actualizarTextoAyudaEdicion();
        return;
    }

    int origen = nodoSeleccionadoEdicion;
    int destino = idNodo;
    limpiarSeleccionEdicion();
    actualizarTextoAyudaEdicion();
    if (origen == destino || g->buscarNodo(origen) == nullptr)
    {
        return;
    }

    QString nombreA = g->buscarNodo(origen)->getNombre();
    QString nombreB = g->buscarNodo(destino)->getNombre();

    if (modoEdicion == 3)
    {
        if (tramoEnUso(origen, destino))
        {
            mostrarMensajeEstado(avisoCalleOcupada());
            return;
        }
        if (!conservarViajesAlEliminar(-1, origen, destino))
        {
            mostrarMensajeEstado(QStringLiteral("Eliminar esta calle dejaría un viaje sin ruta."));
            return;
        }
        bool ok = g->eliminarCalle(origen, destino);
        if (ok)
        {
            actualizarResumenGrafo();
            registrarCambioMapa(QStringLiteral("Calle eliminada"), QStringLiteral("%1 – %2").arg(nombreA, nombreB), origen);
        }
        mostrarMensajeEstado(ok ? QStringLiteral("Calle eliminada entre \"%1\" y \"%2\".").arg(nombreA, nombreB)
                                : QStringLiteral("No existe calle entre \"%1\" y \"%2\".").arg(nombreA, nombreB));
    }
    else if (modoEdicion == 5)
    {
        Arista *arista = g->obtenerArista(origen, destino);
        if (arista == nullptr)
            arista = g->obtenerArista(destino, origen);
        if (arista == nullptr)
        {
            mostrarMensajeEstado(QStringLiteral("No existe calle entre \"%1\" y \"%2\".").arg(nombreA, nombreB));
        }
        else
        {
            bool ok = false;
            double nuevoPeso = QInputDialog::getDouble(
                this, QStringLiteral("Cambiar peso"),
                QStringLiteral("Nuevo peso para la calle entre \"%1\" y \"%2\":").arg(nombreA, nombreB),
                arista->getPeso(), 0.1, 50.0, 1, &ok);
            if (ok)
            {
                if (!g->cambiarPeso(origen, destino, nuevoPeso))
                {
                    mostrarMensajeEstado(QStringLiteral("No se pudo actualizar el peso de la calle."));
                    return;
                }
                registrarCambioMapa(QStringLiteral("Peso de calle actualizado"),
                    QStringLiteral("%1 – %2 · peso %3").arg(nombreA, nombreB).arg(nuevoPeso, 0, 'f', 1), origen);
                mostrarMensajeEstado(QStringLiteral("Peso actualizado a %1.").arg(nuevoPeso));
            }
        }
    }
}

void MainWindow::enNodoEnFocoCambiado(int idNodo)
{
    const Nodo *nodo = sim->getGrafo()->buscarNodo(idNodo);
    const QString detalle = nodo != nullptr
        ? QStringLiteral("Nodo %1 · %2").arg(idNodo).arg(nodo->getNombre())
        : QString();
    ui->lblNodeFocus->setText(ui->lblNodeFocus->fontMetrics().elidedText(
        detalle, Qt::ElideRight, ui->lblNodeFocus->width()));
    ui->lblNodeFocus->setToolTip(detalle);
}

void MainWindow::registrarCambioMapa(QString titulo, QString detalle, int idNodo)
{
    agregarEvento(sim->getReloj().textoHora(), QStringLiteral("Mapa"), titulo, detalle, -1, idNodo);
}

bool MainWindow::mostrarEvento(const EventoRegistro &evento) const
{
    const QString filtro = ui->comboFiltroEventos->currentData().toString();
    if (filtro == QStringLiteral("manual"))
        return evento.getIdTaxi() == sim->getTaxiManual()->getId();
    return filtro.isEmpty() || evento.getCategoria() == filtro;
}

void MainWindow::insertarEvento(const EventoRegistro &evento)
{
    QString texto = QStringLiteral("%1  ·  %2\n%3").arg(evento.getHora(), evento.getCategoria(), evento.getTitulo());
    if (!evento.getDetalle().isEmpty())
        texto += QLatin1Char('\n') + evento.getDetalle();
    QListWidgetItem *item = new QListWidgetItem(texto, ui->listaEventos);
    item->setData(Qt::UserRole, evento.getIdNodo());
    item->setData(Qt::UserRole + 1, evento.getIdentidadNodo());
    const bool vinculoActivo = evento.getIdentidadNodo() != 0
        && evento.getIdentidadNodo() == sim->getGrafo()->getIdentidadNodo(evento.getIdNodo());
    item->setToolTip(vinculoActivo ? QStringLiteral("Pulsa para localizar el cruce en el mapa.") : texto);
}

void MainWindow::actualizarEventos()
{
    ui->listaEventos->clear();
    for (int i = 0; i < eventos.size(); ++i)
    {
        if (mostrarEvento(eventos[i]))
            insertarEvento(eventos[i]);
    }
    const bool hayEventos = ui->listaEventos->count() > 0;
    ui->lblEventosVacio->setVisible(!hayEventos);
    ui->listaEventos->setVisible(hayEventos);
    if (hayEventos)
        ui->listaEventos->scrollToBottom();
}

void MainWindow::agregarEvento(QString hora, QString categoria, QString titulo,
                               QString detalle, int idTaxi, int idNodo)
{
    const EventoRegistro evento(hora, categoria, titulo, detalle, idTaxi, idNodo,
                                sim->getGrafo()->getIdentidadNodo(idNodo));
    eventos.append(evento);
    if (!mostrarEvento(evento))
        return;
    QScrollBar *barra = ui->listaEventos->verticalScrollBar();
    const bool alFinal = barra->value() >= barra->maximum();
    insertarEvento(evento);
    ui->lblEventosVacio->hide();
    ui->listaEventos->show();
    if (alFinal)
        ui->listaEventos->scrollToBottom();
}

void MainWindow::configurarTablas()
{
    QTableWidget *t = ui->tableTaxis;
    t->clear();
    t->setColumnCount(5);
    t->setRowCount(sim->getCantidadTaxis());
    t->setHorizontalHeaderLabels({QStringLiteral("Taxi"), QStringLiteral("Algoritmo"),
                                  QStringLiteral("Viajes"), QStringLiteral("Ingresos"),
                                  QStringLiteral("Nodos")});
    t->horizontalHeaderItem(4)->setToolTip(QStringLiteral("Nodos explorados al calcular rutas"));
    t->verticalHeader()->setDefaultSectionSize(27);
    t->verticalHeader()->setVisible(false);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionMode(QAbstractItemView::SingleSelection);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setAlternatingRowColors(true);
    t->setShowGrid(false);
    t->setWordWrap(false);
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    t->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    t->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    t->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    actualizarTablas();
}

void MainWindow::actualizarResumenGrafo()
{
    const Grafo *grafo = sim->getGrafo();
    ui->lblGraphCounts->setText(QStringLiteral("%1 nodos · %2 aristas")
                                    .arg(grafo->getNodos()->getCantidad())
                                    .arg(grafo->getAristas()->getCantidad()));
}

void MainWindow::actualizarEstadoSimulacion()
{
    actualizarResumenGrafo();
    const RelojSimulacion &reloj = sim->getReloj();
    const QString periodo = reloj.periodoActual();
    const QString estado = periodo == "HoraPico" ? QStringLiteral("Hora pico ×1.8")
                         : periodo == "Nocturno" ? QStringLiteral("Nocturno +$2.000")
                                                 : QStringLiteral("Tráfico normal");
    ui->lblGraphClock->setText(reloj.textoHora());
    const QString siguiente = reloj.textoProximoPeriodo();
    const QString resumen = QStringLiteral("Día %1 · %2").arg(reloj.getDia()).arg(estado);
    ui->lblGraphPeriod->setText(siguiente.isEmpty() ? resumen : resumen + QLatin1Char('\n') + siguiente);
    ui->lblGraphPeriod->setToolTip(siguiente);
    ui->lblGraphClock->setToolTip(siguiente);
    ui->lblGraphPeriod->setAccessibleDescription(siguiente);

    const TaxiManual *taxi = sim->getTaxiManual();
    const int viajes = taxi->getViajes();
    ui->lblManualTrips->setText(viajes == 1 ? QStringLiteral("1 viaje")
                                           : QStringLiteral("%1 viajes").arg(viajes));
    ui->lblManualIncome->setText(formatearPesos(taxi->getIngresos()));
    if (taxi->getLlevaPasajero())
    {
        const Nodo *destino = sim->getGrafo()->buscarNodo(taxi->getDestinoPasajeroABordo());
        ui->lblManualState->setText(QStringLiteral("A bordo"));
        ui->lblManualDetail->setText(destino != nullptr
            ? QStringLiteral("Destino · %1").arg(destino->getNombre())
            : QStringLiteral("Destino fuera del mapa"));
    }
    else if (temporizadorAviso.isActive() && llegadaManualReciente)
    {
        ui->lblManualState->setText(QStringLiteral("Viaje completado"));
        ui->lblManualDetail->setText(avisoTrayecto);
    }
    else
    {
        ui->lblManualState->setText(QStringLiteral("Buscando pasajero"));
        ui->lblManualDetail->setText(QStringLiteral("Busca a una persona en turquesa."));
    }
    if (modoEdicion == 0 && (temporizadorAviso.isActive() || ui->statusbar->currentMessage().isEmpty()))
    {
        actualizarTextoAyudaEdicion();
    }
}

void MainWindow::actualizarTablas()
{
    actualizarEstadoSimulacion();
    const int espera = sim->getPasajerosEnEspera().size();
    const QString pestanaEspera = QStringLiteral("Espera %1").arg(espera);
    if (ui->tabs->tabText(1) != pestanaEspera)
    {
        ui->tabs->setTabText(1, pestanaEspera);
        ui->tabs->setTabToolTip(1, QStringLiteral("%1 pasajeros en espera").arg(espera));
    }
    if (ui->tabs->currentWidget() == ui->tabPassengers)
        actualizarEspera();
    if (ui->tabs->currentWidget() != ui->tabEarnings)
        return;
    QTableWidget *t = ui->tableTaxis;
    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *tx = sim->getTaxi(i);
        asignarCelda(t, i, 0, QStringLiteral("● ") + tx->getNombre(), tx->getColor(), Qt::AlignLeft);
        asignarCelda(t, i, 1, tx->nombreAlgoritmo(), QColor("#dbe3ef"), Qt::AlignLeft);
        asignarCelda(t, i, 2, QString::number(tx->getViajes()), QColor("#dbe3ef"), Qt::AlignRight);
        asignarCelda(t, i, 3, formatearPesos(tx->getIngresos()), QColor("#89e3be"), Qt::AlignRight);
        asignarCelda(t, i, 4,
                     tx->esManual() ? QStringLiteral("-") : QString::number(tx->getNodosExplorados()),
                     QColor("#dbe3ef"), Qt::AlignRight);
    }
}

bool MainWindow::pasajeroSigueEnEspera(int idPasajero) const
{
    const QVector<Pasajero> &lista = sim->getPasajerosEnEspera();
    for (int i = 0; i < lista.size(); ++i)
    {
        if (lista[i].getId() == idPasajero)
            return true;
    }
    return false;
}

void MainWindow::actualizarEspera()
{
    QTableWidget *tp = ui->tablePassengers;
    if (tp->columnCount() != 3)
    {
        tp->setColumnCount(3);
        tp->setHorizontalHeaderLabels({QStringLiteral("Recogida"), QStringLiteral("Destino"),
                                       QStringLiteral("Tarifa estimada")});
        tp->verticalHeader()->setDefaultSectionSize(30);
        tp->verticalHeader()->setVisible(false);
        tp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tp->setSelectionMode(QAbstractItemView::SingleSelection);
        tp->setSelectionBehavior(QAbstractItemView::SelectRows);
        tp->setAlternatingRowColors(true);
        tp->setShowGrid(false);
        tp->setWordWrap(false);
        tp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tp->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    }

    const QVector<Pasajero> &lista = sim->getPasajerosEnEspera();
    const Grafo *g = sim->getGrafo();
    const int revisionPesos = g->getRevisionPesos();
    const QString periodo = sim->getReloj().periodoActual();
    const bool mismasTarifas = revisionPesosCotizados == revisionPesos && periodoCotizado == periodo;
    bool mismosPasajeros = (lista.size() == pasajerosEnTabla.size());
    if (mismosPasajeros)
    {
        for (int i = 0; i < lista.size(); ++i)
        {
            if (lista[i].getId() != pasajerosEnTabla[i])
            {
                mismosPasajeros = false;
                break;
            }
        }
    }
    if (mismosPasajeros && mismasTarifas)
        return;

    const QTableWidgetItem *seleccionado = tp->item(tp->currentRow(), 0);
    const int idSeleccionado = seleccionado != nullptr && seleccionado->isSelected()
        ? seleccionado->data(Qt::UserRole).toInt() : -1;
    int filaSeleccionada = -1;
    if (!mismosPasajeros)
    {
        tp->selectionModel()->clear();
        tp->setRowCount(lista.size());
        pasajerosEnTabla.resize(lista.size());

        const QList<int> claves = tarifasVisibles.keys();
        for (int i = 0; i < claves.size(); ++i)
        {
            if (!pasajeroSigueEnEspera(claves[i]))
            {
                tarifasVisibles.remove(claves[i]);
            }
        }
        if (pasajeroUbicado >= 0 && !pasajeroSigueEnEspera(pasajeroUbicado))
            limpiarUbicacionPasajero();
    }
    ui->lblEsperaVacia->setVisible(lista.isEmpty());
    tp->setVisible(!lista.isEmpty());

    revisionPesosCotizados = revisionPesos;
    periodoCotizado = periodo;
    for (int i = 0; i < lista.size(); i++)
    {
        const Pasajero &p = lista[i];
        const int id = p.getId();
        if (!mismosPasajeros)
        {
            pasajerosEnTabla[i] = id;
            if (id == idSeleccionado)
                filaSeleccionada = i;
            const Nodo *origen = g->buscarNodo(p.getOrigen());
            const Nodo *destino = g->buscarNodo(p.getDestino());
            const QString nombreOrigen = origen != nullptr ? origen->getNombre() : QStringLiteral("?");
            const QString nombreDestino = destino != nullptr ? destino->getNombre() : QStringLiteral("?");
            asignarCelda(tp, i, 0, nombreOrigen, QColor("#dbe3ef"), Qt::AlignLeft);
            asignarCelda(tp, i, 1, nombreDestino, QColor("#dbe3ef"), Qt::AlignLeft);
            tp->item(i, 0)->setData(Qt::UserRole, id);
            tp->item(i, 0)->setToolTip(nombreOrigen);
            tp->item(i, 1)->setToolTip(nombreDestino);
        }
        QString importe;
        if (mismasTarifas && tarifasVisibles.contains(id))
            importe = tarifasVisibles.value(id);
        else
        {
            const double tarifaEstimada = sim->cotizarCarrera(p.getOrigen(), p.getDestino());
            importe = tarifaEstimada > 0.0 ? formatearPesos(tarifaEstimada)
                                           : QStringLiteral("Sin ruta");
            tarifasVisibles.insert(id, importe);
        }
        asignarCelda(tp, i, 2, importe, QColor("#ffd60a"), Qt::AlignRight);
    }
    if (filaSeleccionada >= 0)
        tp->setCurrentCell(filaSeleccionada, 0, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void MainWindow::ubicarPasajero(int fila)
{
    const QTableWidgetItem *recogida = ui->tablePassengers->item(fila, 0);
    if (recogida == nullptr)
        return;

    const int id = recogida->data(Qt::UserRole).toInt();
    const QVector<Pasajero> &lista = sim->getPasajerosEnEspera();
    int indiceEncontrado = -1;
    for (int i = 0; i < lista.size(); ++i)
    {
        if (lista[i].getId() == id)
        {
            indiceEncontrado = i;
            break;
        }
    }
    if (indiceEncontrado >= 0)
    {
        pasajeroUbicado = id;
        mapaWidget->resaltarRecorridoPasajero(lista[indiceEncontrado].getOrigen(),
                                              lista[indiceEncontrado].getDestino());
    }
    else
        actualizarEspera();
}

void MainWindow::limpiarUbicacionPasajero()
{
    if (pasajeroUbicado < 0)
        return;

    pasajeroUbicado = -1;
    mapaWidget->ocultarRecorridoPasajero();
}

void MainWindow::notificarCuadroActualizado(double dtReal)
{
    if (mapaWidget != nullptr)
        mapaWidget->enCuadro(dtReal);
    if (barrasWidget != nullptr)
        barrasWidget->enCuadro(dtReal);
}

void MainWindow::notificarMuestraGrafico(double segundosSimulados)
{
    if (graficoWidget != nullptr)
        graficoWidget->enMuestra(segundosSimulados);
}

void MainWindow::notificarPasajeroRecogido(int idTaxi, int origen, int destino)
{
    if (mapaWidget != nullptr)
        mapaWidget->enPasajeroRecogido(idTaxi, origen, destino);

    if (pasajeroUbicado >= 0 && !pasajeroSigueEnEspera(pasajeroUbicado))
        limpiarUbicacionPasajero();
    if (modoEdicion != 0 || taxiConAviso()->getId() != idTaxi)
        return;
    const Nodo *desde = sim->getGrafo()->buscarNodo(origen);
    const Nodo *hasta = sim->getGrafo()->buscarNodo(destino);
    if (desde == nullptr || hasta == nullptr)
        return;
    llegadaManualReciente = false;
    avisoTrayecto = QStringLiteral("Recogida en %1 · destino: %2").arg(desde->getNombre(), hasta->getNombre());
    temporizadorAviso.start(4200);
    actualizarEstadoSimulacion();
}

void MainWindow::notificarCarreraCompletada(int idTaxi, QPointF posMapa, double tarifa, QColor color)
{
    if (mapaWidget != nullptr)
        mapaWidget->enCarreraCompletada(idTaxi, posMapa, tarifa, color);

    const Taxi *taxi = taxiConAviso();
    if (modoEdicion != 0 || taxi->getId() != idTaxi)
        return;
    const Nodo *destino = sim->getGrafo()->buscarNodo(taxi->getNodoActual());
    if (destino == nullptr)
        return;
    llegadaManualReciente = taxi->esManual();
    avisoTrayecto = QStringLiteral("Llegada a %1 · +%2").arg(destino->getNombre(), formatearPesos(tarifa));
    temporizadorAviso.start(4200);
    actualizarEstadoSimulacion();
}

void MainWindow::notificarReinicioCompletado()
{
    if (mapaWidget != nullptr)
        mapaWidget->enReinicioSimulacion();
    if (barrasWidget != nullptr)
        barrasWidget->reiniciarBarras();
    if (graficoWidget != nullptr)
        graficoWidget->reconstruirSeries();

    temporizadorAviso.stop();
    limpiarSeleccionEdicion();
    limpiarUbicacionPasajero();
    eventos.clear();
    actualizarEventos();
    ui->tablePassengers->selectionModel()->clear();
    ui->tablePassengers->setRowCount(0);
    pasajerosEnTabla.clear();
    tarifasVisibles.clear();
    actualizarTextoAyudaEdicion();
    configurarTablas();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui = new Ui::MainWindow();
    sim = new Simulacion(this);
    mapaWidget = nullptr;
    barrasWidget = nullptr;
    graficoWidget = nullptr;
    modoEdicion = 0;
    nodoSeleccionadoEdicion = -1;

    ui->setupUi(this);
    ui->comboFiltroEventos->addItem(QStringLiteral("Mi taxi"), QStringLiteral("manual"));
    ui->comboFiltroEventos->addItem(QStringLiteral("Viajes de la flota"), QStringLiteral("Viajes"));
    ui->comboFiltroEventos->addItem(QStringLiteral("Tráfico"), QStringLiteral("Tráfico"));
    ui->comboFiltroEventos->addItem(QStringLiteral("Cambios del mapa"), QStringLiteral("Mapa"));
    ui->comboFiltroEventos->addItem(QStringLiteral("Todos"), QString());
    ui->listaEventos->setResizeMode(QListView::Adjust);
    ui->comboMapTaxi->addItem(QStringLiteral("Mi taxi"), sim->getTaxiManual()->getId());
    ui->comboMapTaxi->addItem(QStringLiteral("Toda la flota"), -1);
    for (int i = 0; i < sim->getCantidadTaxis(); ++i)
    {
        const Taxi *taxi = sim->getTaxi(i);
        if (!taxi->esManual())
            ui->comboMapTaxi->addItem(taxi->getNombre(), taxi->getId());
    }
    ui->lblEditHint->setTextFormat(Qt::PlainText);
    ui->lblEditHint->setWordWrap(false);
    ui->layoutEdit->setContentsMargins(0, 1, 0, 5);
    ui->lblManualDetail->setTextFormat(Qt::PlainText);
    ui->lblNodeFocus->setTextFormat(Qt::PlainText);
    ui->lblNodeFocus->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->layoutGraphLegend->setStretch(ui->layoutGraphLegend->indexOf(ui->lblNodeFocus), 1);
    ui->layoutEdit->setStretch(ui->layoutEdit->indexOf(ui->lblEditHint), 1);
    ui->comboTimeMode->setAccessibleName(QStringLiteral("Período de tráfico"));
    QButtonGroup *modosMapa = new QButtonGroup(this);
    modosMapa->addButton(ui->btnDriveMode);
    modosMapa->addButton(ui->btnEditMode);
    ui->btnDriveMode->setChecked(true);

    aplicarSombraFlotante(ui->btnPause, 13, 2, 115);
    aplicarSombraFlotante(ui->btnReset, 11, 2, 100);
    aplicarSombraFlotante(ui->comboSpeed, 11, 2, 100);
    aplicarSombraFlotante(ui->comboTimeMode, 11, 2, 100);
    aplicarSombraFlotante(ui->btnDriveMode, 11, 2, 100);
    aplicarSombraFlotante(ui->btnEditMode, 11, 2, 100);
    aplicarSombraFlotante(ui->comboEditMode, 11, 2, 100);
    aplicarSombraFlotante(ui->comboSentidoCalle, 11, 2, 100);
    aplicarSombraFlotante(ui->btnConstruirCalle, 11, 2, 105);
    aplicarSombraFlotante(ui->comboMapTaxi, 11, 2, 100);
    aplicarSombraFlotante(ui->comboFiltroEventos, 10, 2, 95);

    mapaWidget = new MapaWidget(ui->mapHost);
    mapaWidget->setFocusPolicy(Qt::ClickFocus);
    mapaWidget->setVentanaPrincipal(this);
    ui->layoutMapHost->insertWidget(ui->layoutMapHost->indexOf(ui->graphLegend), mapaWidget, 1);
    barrasWidget = new BarrasGananciasWidget(ui->barsHost);
    ui->barsHost->layout()->addWidget(barrasWidget);
    graficoWidget = new GraficoGananciasWidget(ui->chartHost);
    ui->chartHost->layout()->addWidget(graficoWidget);

    mapaWidget->setSimulacion(sim);
    mapaWidget->setMostrarPesos(ui->chkWeights->isChecked());
    mapaWidget->setTaxiEnFoco(ui->comboMapTaxi->currentData().toInt());
    barrasWidget->setSimulacion(sim);
    graficoWidget->setSimulacion(sim);
    sim->setVentanaPrincipal(this);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);
    ui->splitter->setSizes({1030, 355});

    configurarTablas();
    configurarPanelGuia();
    actualizarEventos();
    enModoEdicionCambiado(0);
    actualizarResumenGrafo();

    pestanaAnterior = ui->tabs->currentWidget();
    indicePestanaAnterior = ui->tabs->currentIndex();
    capaTransicionPestana = new QLabel(ui->tabs->currentWidget()->parentWidget());
    capaTransicionPestana->setAttribute(Qt::WA_TransparentForMouseEvents);
    capaTransicionPestana->hide();
    efectoOpacidadPestana = new QGraphicsOpacityEffect(capaTransicionPestana);
    capaTransicionPestana->setGraphicsEffect(efectoOpacidadPestana);

    animacionPestana.setStartValue(0.0);
    animacionPestana.setEndValue(1.0);
    animacionPestana.setDuration(150);
    animacionPestana.setEasingCurve(QEasingCurve::OutCubic);
    connect(&animacionPestana, &QVariantAnimation::valueChanged, this, &MainWindow::enPasoAnimacionPestana);
    connect(&animacionPestana, &QVariantAnimation::finished, this, &MainWindow::enFinAnimacionPestana);

    connect(ui->btnPause, &QPushButton::toggled, this, &MainWindow::enPausaCambiada);
    connect(ui->btnReset, &QPushButton::clicked, this, &MainWindow::enReiniciarClicado);
    connect(ui->comboSpeed, &QComboBox::currentIndexChanged, this, &MainWindow::enVelocidadCambiada);
    connect(ui->comboTimeMode, &QComboBox::currentIndexChanged, this, &MainWindow::enModoTiempoCambiado);
    connect(ui->btnDriveMode, &QPushButton::clicked, this, &MainWindow::enModoConducirClicado);
    connect(ui->btnEditMode, &QPushButton::clicked, this, &MainWindow::enModoEditarClicado);
    connect(ui->comboEditMode, &QComboBox::currentIndexChanged, this, &MainWindow::enHerramientaEdicionCambiada);
    connect(ui->comboSentidoCalle, &QComboBox::currentIndexChanged, this, &MainWindow::enSentidoCalleCambiado);
    connect(ui->btnConstruirCalle, &QPushButton::clicked, this, &MainWindow::confirmarCalle);

    const Qt::Key teclasConfirmar[2] = {Qt::Key_Return, Qt::Key_Enter};
    for (int i = 0; i < 2; ++i)
    {
        QShortcut *confirmar = new QShortcut(QKeySequence(teclasConfirmar[i]), ui->mapHost);
        confirmar->setContext(Qt::WidgetWithChildrenShortcut);
        confirmar->setEnabled(false);
        atajosConfirmar.append(confirmar);
        connect(confirmar, &QShortcut::activated, this, &MainWindow::enAtajoConfirmarCalle);
    }
    QShortcut *cancelar = new QShortcut(QKeySequence(Qt::Key_Escape), ui->centralwidget);
    cancelar->setContext(Qt::WidgetWithChildrenShortcut);
    connect(cancelar, &QShortcut::activated, this, &MainWindow::enCancelarClicado);
    connect(ui->chkWeights, &QCheckBox::toggled, this, &MainWindow::enMostrarPesosCambiado);
    connect(ui->tableTaxis, &QTableWidget::cellClicked, this, &MainWindow::enFilaTaxiSeleccionada);
    connect(ui->tableTaxis, &QTableWidget::cellActivated, this, &MainWindow::enFilaTaxiSeleccionada);
    connect(ui->tablePassengers, &QTableWidget::cellClicked, this, &MainWindow::ubicarPasajero);
    connect(ui->tablePassengers, &QTableWidget::cellActivated, this, &MainWindow::ubicarPasajero);
    connect(ui->comboMapTaxi, &QComboBox::currentIndexChanged, this, &MainWindow::enTaxiMapaCambiado);

    connect(ui->comboFiltroEventos, &QComboBox::currentIndexChanged, this, &MainWindow::actualizarEventos);
    connect(ui->listaEventos, &QListWidget::itemClicked, this, &MainWindow::enEventoListaSeleccionado);
    connect(ui->listaEventos, &QListWidget::itemActivated, this, &MainWindow::enEventoListaSeleccionado);

    temporizadorAviso.setSingleShot(true);
    connect(&temporizadorAviso, &QTimer::timeout, this, &MainWindow::actualizarEstadoSimulacion);

    temporizadorResalteIngreso.setSingleShot(true);
    temporizadorResalteIngreso.setInterval(650);
    connect(&temporizadorResalteIngreso, &QTimer::timeout, this, &MainWindow::restaurarEstiloIngresoManual);

    temporizadorTablas.setInterval(250);
    connect(&temporizadorTablas, &QTimer::timeout, this, &MainWindow::actualizarTablas);
    temporizadorTablas.start();
    connect(ui->tabs, &QTabWidget::currentChanged, this, &MainWindow::enPestanaCambiada);

    sim->iniciar();
}

void MainWindow::resaltarIngresoManual()
{
    if (ui->lblManualIncome == nullptr)
        return;
    ui->lblManualIncome->setStyleSheet(QStringLiteral("color: #ffd60a; font-weight: bold; font-size: 19px;"));
    temporizadorResalteIngreso.start(650);
}

void MainWindow::restaurarEstiloIngresoManual()
{
    if (ui->lblManualIncome != nullptr)
        ui->lblManualIncome->setStyleSheet(QString());
}

MainWindow::~MainWindow()
{
    if (sim != nullptr)
        sim->setVentanaPrincipal(nullptr);
    if (mapaWidget != nullptr)
        mapaWidget->setVentanaPrincipal(nullptr);
    delete ui;
    ui = nullptr;
}
