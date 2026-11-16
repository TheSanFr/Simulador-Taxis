#include "MainWindow.h"

#include "Arista.h"
#include "BarrasGananciasWidget.h"
#include "DialogoNodo.h"
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
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QTextEdit>
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
    item->setText(texto);
    item->setForeground(color);
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
        texto = QStringLiteral("Pulsa un espacio libre para ubicar el nodo.");
    else if (modoEdicion == 2)
        texto = nodoSeleccionadoEdicion < 0 ? QStringLiteral("Selecciona el cruce de origen.")
                                             : QStringLiteral("Selecciona el cruce de destino.");
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
<h3 style="color:#f4f7fa">Tu primera carrera</h3>
<p>En Conducir, pulsa un cruce resaltado para avanzar. Las personas turquesa esperan en la calle.
El primer taxi vacío que llega las recoge. Sigue la bandera para completar el viaje.
Usa clic derecho para cancelar los tramos en cola.</p>
<h3 style="color:#f4f7fa">Comparar algoritmos</h3>
<p><b>BFS</b> busca una ruta con menos saltos. <b>DFS</b> explora una rama a fondo.
<b>Dijkstra</b> busca una ruta de menor peso. Elige un taxi en Rutas o en la tabla de flota para ver su recorrido.</p>
<h3 style="color:#f4f7fa">Reglas de la ciudad</h3>
<ul>
<li><b>Ciudad y Grafo:</b> Grafo dirigido y ponderado (<code>class Grafo</code> con <code>ListaNodos</code> y <code>ListaAristas</code>) donde las calles iniciales son de doble sentido (dos aristas dirigidas A→B y B→A) y es modificable por interfaz y por código.</li>
<li><b>Taxis:</b> 6 autónomos (2 con <b>BFS</b>, 2 con <b>DFS</b>, 2 con <b>Dijkstra</b>) y 1 controlado por el usuario (<b>Tú</b>).</li>
<li><b>Fórmula del Taxímetro (Mín. $8,100):</b><br>
<code>Taxímetro = Base ($4,600) + Distancia (saltos) × $1,625 + Tiempo (ponderado) × $1,625</code></li>
<li><b>Destinos Especiales:</b> Aeropuerto (+$8,600), Terminal (+$2,400), Sitios turísticos (+$2,000 y triple probabilidad de ser destino).</li>
<li><b>Tráfico por uso:</b> Cada vez que un taxi cruza una arista, el peso de esa arista aumenta (+0.2).</li>
<li><b>Modos:</b> Nocturno (+$2,000 de recargo) y Hora Pico (ponderaciones ×1.8).</li>
</ul>
)"));
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
    modoEdicion = indice;
    nodoSeleccionadoEdicion = -1;
    mapaWidget->setNodoPendiente(-1);
    mapaWidget->setModoConducir(modoEdicion == 0);
    ui->comboEditMode->setVisible(modoEdicion != 0);
    ui->lblMapTaxi->setVisible(modoEdicion == 0);
    ui->comboMapTaxi->setVisible(modoEdicion == 0);
    actualizarTextoAyudaEdicion();
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
        nodoSeleccionadoEdicion = -1;
        mapaWidget->setNodoPendiente(-1);
        actualizarTextoAyudaEdicion();
    }
}

void MainWindow::enEspacioVacioClicado(QPointF posMapa)
{
    if (modoEdicion != 1)
    {
        return;
    }

    bool estabaPausada = sim->estaPausada();
    sim->setPausada(true);
    DialogoNodo dialogo(this);
    if (dialogo.exec() == QDialog::Accepted)
    {
        QString nombre = dialogo.getNombreNodo();
        if (nombre.isEmpty())
        {
            nombre = QStringLiteral("Nuevo nodo");
        }
        sim->getGrafo()->agregarNodoNuevo(nombre, posMapa, dialogo.getTipoNodo());
        mostrarMensajeEstado(
            QStringLiteral("Nodo \"%1\" creado. Conéctalo usando \"Agregar calle\".").arg(nombre));
    }
    sim->setPausada(estabaPausada);
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
        bool estabaPausada = sim->estaPausada();
        sim->setPausada(true);
        QString nombre = nodoClic->getNombre();
        if (QMessageBox::question(this, QStringLiteral("Eliminar nodo"),
                                  QStringLiteral("¿Eliminar \"%1\" y sus calles?").arg(nombre))
            == QMessageBox::Yes)
        {
            g->eliminarNodo(idNodo);
            mostrarMensajeEstado(QStringLiteral("Nodo \"%1\" eliminado.").arg(nombre));
        }
        sim->setPausada(estabaPausada);
        return;
    }

    if (nodoSeleccionadoEdicion < 0)
    {
        nodoSeleccionadoEdicion = idNodo;
        mapaWidget->setNodoPendiente(idNodo);
        actualizarTextoAyudaEdicion();
        return;
    }

    int origen = nodoSeleccionadoEdicion;
    int destino = idNodo;
    nodoSeleccionadoEdicion = -1;
    mapaWidget->setNodoPendiente(-1);
    actualizarTextoAyudaEdicion();
    if (origen == destino || g->buscarNodo(origen) == nullptr)
    {
        return;
    }

    bool estabaPausada = sim->estaPausada();
    sim->setPausada(true);
    QString nombreA = g->buscarNodo(origen)->getNombre();
    QString nombreB = g->buscarNodo(destino)->getNombre();

    if (modoEdicion == 2)
    {
        QMessageBox cuadro(QMessageBox::Question, QStringLiteral("Nueva calle"),
                           QStringLiteral("¿Cómo deseas conectar \"%1\" y \"%2\"?").arg(nombreA, nombreB),
                           QMessageBox::NoButton, this);
        QAbstractButton *btnDoble = cuadro.addButton(QStringLiteral("Doble sentido"), QMessageBox::AcceptRole);
        QAbstractButton *btnUnico = cuadro.addButton(
            QStringLiteral("Un sentido (%1 → %2)").arg(nombreA, nombreB), QMessageBox::AcceptRole);
        cuadro.addButton(QStringLiteral("Cancelar"), QMessageBox::RejectRole);
        cuadro.exec();

        if (cuadro.clickedButton() == btnDoble)
        {
            g->agregarCalleDoble(origen, destino);
            mostrarMensajeEstado(
                QStringLiteral("Calle de doble sentido creada entre \"%1\" y \"%2\".").arg(nombreA, nombreB));
        }
        else if (cuadro.clickedButton() == btnUnico)
        {
            g->agregarArista(origen, destino);
            mostrarMensajeEstado(QStringLiteral("Arista dirigida creada: %1 → %2.").arg(nombreA, nombreB));
        }
    }
    else if (modoEdicion == 3)
    {
        bool ok = g->eliminarCalle(origen, destino);
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
                mostrarMensajeEstado(QStringLiteral("Peso actualizado a %1.").arg(nuevoPeso));
            }
        }
    }

    sim->setPausada(estabaPausada);
}

void MainWindow::agregarEvento(QString texto, QColor color)
{
    ui->txtLog->append(QStringLiteral("<span style=\"color:%1\">%2</span>")
                           .arg(color.name(), texto.toHtmlEscaped()));
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
    ui->lblGraphPeriod->setText(QStringLiteral("Día %1 · %2").arg(reloj.getDia()).arg(estado));

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
    for (int i = 0; i < lista.size(); i++)
    {
        const Pasajero &p = lista[i];
        Nodo *no = g->buscarNodo(p.getOrigen());
        Nodo *nd = g->buscarNodo(p.getDestino());
        asignarCelda(tp, i, 0, (no != nullptr) ? no->getNombre() : QStringLiteral("?"),
                     QColor("#dbe3ef"), Qt::AlignLeft);
        asignarCelda(tp, i, 1, (nd != nullptr) ? nd->getNombre() : QStringLiteral("?"),
                     QColor("#dbe3ef"), Qt::AlignLeft);
        asignarCelda(tp, i, 2,
                     Grafo::formatearPesos(sim->cotizarCarrera(p.getOrigen(), p.getDestino())),
                     QColor("#ffd60a"), Qt::AlignRight);
    }

    ui->tabs->setTabText(1, QStringLiteral("Espera %1").arg(lista.size()));
    ui->tabs->setTabToolTip(1, QStringLiteral("%1 pasajeros en espera").arg(lista.size()));
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
    enModoEdicionCambiado(0);
    actualizarResumenGrafo();

    connect(sim->getGrafo(), &Grafo::grafoModificado, this, &MainWindow::actualizarResumenGrafo);
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

    connect(sim, &Simulacion::mensajeEvento, this, &MainWindow::agregarEvento);
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
        actualizarTextoAyudaEdicion();
    });
    connect(sim, &Simulacion::reinicioCompletado, this, &MainWindow::configurarTablas);
    connect(sim, &Simulacion::pausaCambiada, this, &MainWindow::actualizarBotonPausa);

    temporizadorTablas.setInterval(250);
    connect(&temporizadorTablas, &QTimer::timeout, this, &MainWindow::actualizarTablas);
    temporizadorTablas.start();

    agregarEvento(
        QStringLiteral("Simulación iniciada."),
        QColor("#ffd60a"));
    sim->iniciar();
}

MainWindow::~MainWindow()
{
    delete ui;
    ui = nullptr;
}
