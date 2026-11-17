#include "MainWindow.h"

#include "Arista.h"
#include "BarrasGananciasWidget.h"
#include "DialogoNodo.h"
#include "ElementoArista.h"
#include "GraficoGananciasWidget.h"
#include "Grafo.h"
#include "ListaAristas.h"
#include "ListaNodos.h"
#include "MapaWidget.h"
#include "Nodo.h"
#include "Pasajero.h"
#include "RelojSimulacion.h"
#include "Simulacion.h"
#include "Taxi.h"
#include "TaxiManual.h"
#include "TransicionesPanel.h"
#include "ui_mainwindow.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFontMetrics>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
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

namespace
{
QPixmap marcaGrafo()
{
    QPixmap imagen(64, 64);
    imagen.setDevicePixelRatio(2.0);
    imagen.fill(Qt::transparent);

    QPainter p(&imagen);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor("#3d4f60"), 1));
    p.setBrush(QColor("#202e3a"));
    p.drawRoundedRect(QRectF(1.5, 1.5, 29, 29), 4, 4);

    const QPointF origen(8.5, 19.5);
    const QPointF norte(13.5, 9.5);
    const QPointF cruce(19.5, 16.5);
    const QPointF destino(24, 23);
    p.setPen(QPen(QColor("#93a9be"), 1.3, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(origen, norte);
    p.drawLine(origen, cruce);
    p.drawLine(norte, cruce);
    p.drawLine(cruce, destino);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#cbd9e5"));
    p.drawEllipse(origen, 2, 2);
    p.drawEllipse(norte, 2, 2);
    p.drawEllipse(cruce, 2, 2);
    p.setBrush(QColor("#ffd166"));
    p.drawEllipse(destino, 2.5, 2.5);
    return imagen;
}
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
    if (item->textAlignment() != (alineacion | Qt::AlignVCenter))
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
            const Grafo::RevisionCalle revision = sim->getGrafo()->previsualizarCalle(
                nodoSeleccionadoEdicion, destinoEdicion, ui->comboSentidoCalle->currentIndex() == 0);
            if (!revision.valida)
                texto = revision.motivo;
            else
            {
                puedeConstruir = true;
                texto = revision.cruces.isEmpty()
                    ? QStringLiteral("Peso inicial %1 · Enter para construir.").arg(revision.peso, 0, 'f', 1)
                    : QStringLiteral("Peso inicial %1 · cruces intermedios: %2 · Enter para construir.")
                        .arg(revision.peso, 0, 'f', 1).arg(revision.cruces.size());
            }
            for (const auto &tramo : revision.tramosAfectados)
            {
                if (revision.valida && tramoEnUso(tramo.first, tramo.second))
                {
                    texto = QStringLiteral("Un taxi está cruzando la calle. Reanuda y vuelve a editar cuando termine.");
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
    ui->lblEditHint->setText(texto);
    ui->lblEditHint->setToolTip(texto);
}

void MainWindow::mostrarMensajeEstado(QString texto)
{
    ui->statusbar->showMessage(texto, 8000);
    ui->lblEditHint->setText(texto);
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
<li>La tarifa se fija al recoger al pasajero. Se calcula con base de $4.600, $1.625 por salto y
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
    const Grafo::RevisionNodo revision = grafo->previsualizarNodo(posMapa);
    if (!revision.valida)
    {
        mostrarMensajeEstado(revision.motivo);
        return;
    }
    for (const auto &tramo : revision.tramosAfectados)
    {
        if (tramoEnUso(tramo.first, tramo.second))
        {
            mostrarMensajeEstado(QStringLiteral("Un taxi está cruzando esta calle. Reanuda y vuelve a intentarlo."));
            return;
        }
    }
    DialogoNodo dialogo(this);
    if (revision.divideCalle)
        dialogo.setWindowTitle(QStringLiteral("Crear cruce en la calle"));
    if (dialogo.exec() == QDialog::Accepted)
    {
        QString nombre = dialogo.getNombreNodo();
        if (nombre.isEmpty())
        {
            nombre = revision.divideCalle ? QStringLiteral("Nuevo cruce") : QStringLiteral("Nuevo nodo");
        }
        QString motivo;
        const int id = grafo->crearNodoEditado(posMapa, nombre, dialogo.getTipoNodo(), revision.divideCalle, &motivo);
        if (id < 0)
        {
            mostrarMensajeEstado(motivo);
            return;
        }
        mapaWidget->resaltarNodo(id);
        const QString titulo = revision.divideCalle ? QStringLiteral("Cruce creado") : QStringLiteral("Nodo creado");
        registrarCambioMapa(titulo, nombre, id);
        mostrarMensajeEstado(revision.divideCalle
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

bool MainWindow::conservarViajesAlEliminar(int nodo, int extremoA, int extremoB) const
{
    const Grafo *grafo = sim->getGrafo();
    const auto alcanzable = [&](int inicio, int fin, bool excluir) {
        QVector<int> pendientes{inicio};
        QSet<int> visitados{inicio};
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
                if (excluir && (siguiente == nodo || tramoExcluido))
                    continue;
                if (!visitados.contains(siguiente))
                {
                    visitados.insert(siguiente);
                    pendientes.append(siguiente);
                }
            }
        }
        return false;
    };
    const auto seConserva = [&](int inicio, int fin) {
        return !alcanzable(inicio, fin, false) || alcanzable(inicio, fin, true);
    };
    for (const Pasajero &pasajero : sim->getPasajerosEnEspera())
    {
        if (!seConserva(pasajero.getOrigen(), pasajero.getDestino()))
            return false;
    }
    for (int i = 0; i < sim->getCantidadTaxis(); ++i)
    {
        const Taxi *taxi = sim->getTaxi(i);
        const int partida = taxi->getEnMovimiento() ? taxi->getNodoSiguiente() : taxi->getNodoActual();
        if (taxi->getLlevaPasajero() && !seConserva(partida, taxi->getDestinoPasajeroABordo()))
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
    const Grafo::RevisionCalle revision = grafo->previsualizarCalle(origen, destino, doble);
    if (!revision.valida)
    {
        mostrarMensajeEstado(revision.motivo);
        return;
    }
    for (const auto &tramo : revision.tramosAfectados)
    {
        if (tramoEnUso(tramo.first, tramo.second))
        {
            mostrarMensajeEstado(QStringLiteral("Un taxi está cruzando la calle. Reanuda y vuelve a intentarlo."));
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
    limpiarSeleccionEdicion();
    actualizarTextoAyudaEdicion();
    mapaWidget->resaltarCalle(origen, destino);
    QString detalle = QStringLiteral("%1 · peso %2").arg(enlace).arg(revision.peso, 0, 'f', 1);
    if (!revision.cruces.isEmpty())
        detalle += QStringLiteral(" · cruces intermedios: %1").arg(revision.cruces.size());
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
            mostrarMensajeEstado(QStringLiteral("Hay un taxi en esta calle. Reanuda y vuelve a intentarlo."));
            return;
        }
        if (!conservarViajesAlEliminar(-1, origen, destino))
        {
            mostrarMensajeEstado(QStringLiteral("Eliminar esta calle dejaría un viaje sin ruta."));
            return;
        }
        bool ok = g->eliminarCalle(origen, destino);
        if (ok)
            registrarCambioMapa(QStringLiteral("Calle eliminada"), QStringLiteral("%1 – %2").arg(nombreA, nombreB), origen);
        mostrarMensajeEstado(ok ? QStringLiteral("Calle eliminada entre \"%1\" y \"%2\".").arg(nombreA, nombreB)
                                : QStringLiteral("No existe calle entre \"%1\" y \"%2\".").arg(nombreA, nombreB));
    }
    else if (modoEdicion == 5)
    {
        Arista *arista = g->obtenerArista(origen, destino);
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
                g->cambiarPeso(origen, destino, nuevoPeso);
                registrarCambioMapa(QStringLiteral("Peso de calle actualizado"),
                    QStringLiteral("%1 – %2 · peso %3").arg(nombreA, nombreB).arg(nuevoPeso, 0, 'f', 1), origen);
                mostrarMensajeEstado(QStringLiteral("Peso actualizado a %1.").arg(nuevoPeso));
            }
        }
    }

}

void MainWindow::registrarCambioMapa(QString titulo, QString detalle, int idNodo)
{
    agregarEvento(sim->getReloj().textoHora(), QStringLiteral("Mapa"), titulo, detalle, -1, idNodo);
}

bool MainWindow::mostrarEvento(const Evento &evento) const
{
    const QString filtro = ui->comboFiltroEventos->currentData().toString();
    if (filtro == QStringLiteral("manual"))
        return evento.idTaxi == sim->getTaxiManual()->getId();
    return filtro.isEmpty() || evento.categoria == filtro;
}

void MainWindow::insertarEvento(const Evento &evento)
{
    QString texto = QStringLiteral("%1  ·  %2\n%3").arg(evento.hora, evento.categoria, evento.titulo);
    if (!evento.detalle.isEmpty())
        texto += QLatin1Char('\n') + evento.detalle;
    auto *item = new QListWidgetItem(texto, ui->listaEventos);
    item->setData(Qt::UserRole, evento.idNodo);
    item->setToolTip(evento.idNodo >= 0 ? QStringLiteral("Pulsa para localizar el cruce en el mapa.") : texto);
}

void MainWindow::actualizarEventos()
{
    ui->listaEventos->clear();
    for (const Evento &evento : eventos)
    {
        if (mostrarEvento(evento))
            insertarEvento(evento);
    }
    const bool hayEventos = ui->listaEventos->count() > 0;
    ui->lblEventosVacio->setVisible(!hayEventos);
    ui->listaEventos->setVisible(hayEventos);
    if (hayEventos)
        ui->listaEventos->scrollToBottom();
}

void MainWindow::agregarEvento(QString hora, QString categoria, QString titulo, QString detalle, int idTaxi, int idNodo)
{
    const Evento evento{hora, categoria, titulo, detalle, idTaxi, idNodo};
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
    ui->lblManualIncome->setText(Grafo::formatearPesos(taxi->getIngresos()));
    if (taxi->getLlevaPasajero())
    {
        const Nodo *destino = sim->getGrafo()->buscarNodo(taxi->getDestinoPasajeroABordo());
        ui->lblManualState->setText(QStringLiteral("A bordo"));
        ui->lblManualDetail->setText(destino != nullptr
            ? QStringLiteral("%1 · tarifa %2").arg(destino->getNombre(),
                Grafo::formatearPesos(taxi->getTarifaPasajeroABordo()))
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
        asignarCelda(t, i, 3, Grafo::formatearPesos(tx->getIngresos()), QColor("#89e3be"), Qt::AlignRight);
        asignarCelda(t, i, 4,
                     tx->esManual() ? QStringLiteral("-") : QString::number(tx->getNodosExplorados()),
                     QColor("#dbe3ef"), Qt::AlignRight);
    }
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
        tp->setSelectionMode(QAbstractItemView::NoSelection);
        tp->setAlternatingRowColors(true);
        tp->setShowGrid(false);
        tp->setWordWrap(false);
        tp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }

    const QVector<Pasajero> &lista = sim->getPasajerosEnEspera();
    tp->setRowCount(lista.size());
    Grafo *g = sim->getGrafo();
    if (lista.isEmpty())
    {
        tarifasVisibles.clear();
        return;
    }
    const QString periodo = sim->getReloj().periodoActual();
    QVector<double> pesos;
    pesos.reserve(g->getAristas()->getCantidad());
    for (const ElementoArista *e = g->getAristas()->getPrimero(); e != nullptr; e = e->getSiguiente())
        pesos.append(g->obtenerPesoEfectivoArista(e->getArista()));
    if (pesosCotizados != pesos || periodoCotizado != periodo)
    {
        pesosCotizados = std::move(pesos);
        periodoCotizado = periodo;
        tarifasVisibles.clear();
    }
    QHash<int, QString> tarifasActuales;
    tarifasActuales.reserve(lista.size());
    for (int i = 0; i < lista.size(); i++)
    {
        const Pasajero &p = lista[i];
        Nodo *no = g->buscarNodo(p.getOrigen());
        Nodo *nd = g->buscarNodo(p.getDestino());
        asignarCelda(tp, i, 0, (no != nullptr) ? no->getNombre() : QStringLiteral("?"),
                     QColor("#dbe3ef"), Qt::AlignLeft);
        asignarCelda(tp, i, 1, (nd != nullptr) ? nd->getNombre() : QStringLiteral("?"),
                     QColor("#dbe3ef"), Qt::AlignLeft);
        const auto tarifa = tarifasVisibles.constFind(p.getId());
        const QString importe = tarifa != tarifasVisibles.cend() ? tarifa.value()
            : Grafo::formatearPesos(sim->cotizarCarrera(p.getOrigen(), p.getDestino()));
        tarifasActuales.insert(p.getId(), importe);
        asignarCelda(tp, i, 2, importe, QColor("#ffd60a"), Qt::AlignRight);
    }
    tarifasVisibles.swap(tarifasActuales);
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
    const QPixmap marca = marcaGrafo();
    ui->lblGraphMark->setPixmap(marca);
    setWindowIcon(QIcon(marca));
    ui->lblEditHint->setTextFormat(Qt::PlainText);
    ui->lblManualDetail->setTextFormat(Qt::PlainText);
    ui->lblNodeFocus->setTextFormat(Qt::PlainText);
    ui->lblNodeFocus->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->layoutGraphLegend->setStretch(ui->layoutGraphLegend->indexOf(ui->lblNodeFocus), 1);
    ui->layoutEdit->setStretch(ui->layoutEdit->indexOf(ui->lblEditHint), 1);
    ui->comboTimeMode->setAccessibleName(QStringLiteral("Período de tráfico"));
    auto *modosMapa = new QButtonGroup(this);
    modosMapa->addButton(ui->btnDriveMode);
    modosMapa->addButton(ui->btnEditMode);
    ui->btnDriveMode->setChecked(true);

    mapaWidget = new MapaWidget(ui->mapHost);
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

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);
    ui->splitter->setSizes({1030, 355});

    configurarTablas();
    configurarPanelGuia();
    actualizarEventos();
    enModoEdicionCambiado(0);
    actualizarResumenGrafo();

    connect(sim->getGrafo(), &Grafo::grafoModificado, this, &MainWindow::actualizarResumenGrafo);
    connect(sim->getGrafo(), &Grafo::grafoModificado, this, [this] {
        tarifasVisibles.clear();
    });
    connect(mapaWidget, &MapaWidget::nodoEnFocoCambiado, this, [this](int id) {
        const Nodo *nodo = sim->getGrafo()->buscarNodo(id);
        const QString detalle = nodo != nullptr
            ? QStringLiteral("Nodo %1 · %2").arg(id).arg(nodo->getNombre())
            : QString();
        ui->lblNodeFocus->setText(ui->lblNodeFocus->fontMetrics().elidedText(
            detalle, Qt::ElideRight, ui->lblNodeFocus->width()));
        ui->lblNodeFocus->setToolTip(detalle);
    });
    connect(ui->btnPause, &QPushButton::toggled, this, &MainWindow::enPausaCambiada);
    connect(ui->btnReset, &QPushButton::clicked, this, &MainWindow::enReiniciarClicado);
    connect(ui->comboSpeed, &QComboBox::currentIndexChanged, this, &MainWindow::enVelocidadCambiada);
    connect(ui->comboTimeMode, &QComboBox::currentIndexChanged, this, &MainWindow::enModoTiempoCambiado);
    connect(ui->btnDriveMode, &QPushButton::clicked, this, [this] { enModoEdicionCambiado(0); });
    connect(ui->btnEditMode, &QPushButton::clicked, this, [this] {
        enModoEdicionCambiado(ui->comboEditMode->currentIndex() + 1);
    });
    connect(ui->comboEditMode, &QComboBox::currentIndexChanged, this, [this](int indice) {
        if (ui->btnEditMode->isChecked())
            enModoEdicionCambiado(indice + 1);
    });
    connect(ui->comboSentidoCalle, &QComboBox::currentIndexChanged, this, [this](int indice) {
        mapaWidget->setSentidoCalle(indice == 0);
        actualizarTextoAyudaEdicion();
    });
    connect(ui->btnConstruirCalle, &QPushButton::clicked, this, &MainWindow::confirmarCalle);
    for (const Qt::Key tecla : {Qt::Key_Return, Qt::Key_Enter})
    {
        auto *confirmar = new QShortcut(QKeySequence(tecla), ui->centralwidget);
        confirmar->setContext(Qt::WidgetWithChildrenShortcut);
        connect(confirmar, &QShortcut::activated, this, [this] {
            if (modoEdicion == 2 && ui->btnConstruirCalle->isEnabled())
                confirmarCalle();
        });
    }
    auto *cancelar = new QShortcut(QKeySequence(Qt::Key_Escape), ui->centralwidget);
    cancelar->setContext(Qt::WidgetWithChildrenShortcut);
    connect(cancelar, &QShortcut::activated, this, &MainWindow::enCancelarClicado);
    connect(ui->chkWeights, &QCheckBox::toggled, this, &MainWindow::enMostrarPesosCambiado);
    const auto enfocarFila = [this](int fila, int) {
        if (fila < 0 || fila >= sim->getCantidadTaxis())
            return;
        const Taxi *taxi = sim->getTaxi(fila);
        const int indice = ui->comboMapTaxi->findData(taxi->getId());
        if (indice >= 0)
            ui->comboMapTaxi->setCurrentIndex(indice);
    };
    connect(ui->tableTaxis, &QTableWidget::cellClicked, this, enfocarFila);
    connect(ui->tableTaxis, &QTableWidget::cellActivated, this, enfocarFila);
    connect(ui->comboMapTaxi, &QComboBox::currentIndexChanged, this, [this] {
        temporizadorAviso.stop();
        mapaWidget->setTaxiEnFoco(ui->comboMapTaxi->currentData().toInt());
        actualizarEstadoSimulacion();
    });

    connect(mapaWidget, &MapaWidget::nodoClicado, this, &MainWindow::enNodoClicado);
    connect(mapaWidget, &MapaWidget::espacioVacioClicado, this, &MainWindow::enEspacioVacioClicado);
    connect(mapaWidget, &MapaWidget::cancelarSolicitado, this, &MainWindow::enCancelarClicado);

    connect(sim, &Simulacion::eventoOcurrido, this, &MainWindow::agregarEvento);
    connect(ui->comboFiltroEventos, &QComboBox::currentIndexChanged, this, &MainWindow::actualizarEventos);
    const auto ubicarEvento = [this](QListWidgetItem *item) {
        const int id = item->data(Qt::UserRole).toInt();
        if (id >= 0 && sim->getGrafo()->tieneNodo(id))
            mapaWidget->resaltarNodo(id);
    };
    connect(ui->listaEventos, &QListWidget::itemClicked, this, ubicarEvento);
    connect(ui->listaEventos, &QListWidget::itemActivated, this, ubicarEvento);
    temporizadorAviso.setSingleShot(true);
    connect(&temporizadorAviso, &QTimer::timeout, this, &MainWindow::actualizarEstadoSimulacion);
    connect(sim, &Simulacion::pasajeroRecogido, this, [this](int idTaxi, int origen, int destino) {
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
    });
    connect(sim, &Simulacion::carreraCompletada, this, [this](int idTaxi, QPointF, double tarifa, QColor) {
        const Taxi *taxi = taxiConAviso();
        if (modoEdicion != 0 || taxi->getId() != idTaxi)
            return;
        const Nodo *destino = sim->getGrafo()->buscarNodo(taxi->getNodoActual());
        if (destino == nullptr)
            return;
        llegadaManualReciente = taxi->esManual();
        avisoTrayecto = QStringLiteral("Llegada a %1 · +%2").arg(destino->getNombre(), Grafo::formatearPesos(tarifa));
        temporizadorAviso.start(4200);
        actualizarEstadoSimulacion();
    });
    connect(sim, &Simulacion::reinicioCompletado, this, [this] {
        temporizadorAviso.stop();
        limpiarSeleccionEdicion();
        eventos.clear();
        actualizarEventos();
        actualizarTextoAyudaEdicion();
    });
    connect(sim, &Simulacion::reinicioCompletado, this, &MainWindow::configurarTablas);
    connect(sim, &Simulacion::pausaCambiada, this, &MainWindow::actualizarBotonPausa);

    temporizadorTablas.setInterval(250);
    connect(&temporizadorTablas, &QTimer::timeout, this, &MainWindow::actualizarTablas);
    temporizadorTablas.start();
    connect(ui->tabs, &QTabWidget::currentChanged, this, &MainWindow::actualizarTablas);
    new TransicionesPanel(ui->tabs);

    sim->iniciar();
}

MainWindow::~MainWindow()
{
    delete ui;
    ui = nullptr;
}
