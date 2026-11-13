#define COMPILAR_MOCS

#include <QApplication>
#include <QString>

#include "Nodo.cpp"
#include "Arista.cpp"
#include "ElementoNodo.cpp"
#include "ListaNodos.cpp"
#include "ElementoArista.cpp"
#include "ListaAristas.cpp"
#include "NodoCola.cpp"
#include "Cola.cpp"
#include "NodoPila.cpp"
#include "Pila.cpp"
#include "ResultadoRuta.cpp"
#include "Pasajero.cpp"
#include "ManzanaCiudad.cpp"
#include "TextoFlotante.cpp"
#include "Grafo.cpp"
#include "RelojSimulacion.cpp"
#include "Taxi.cpp"
#include "TaxiBFS.cpp"
#include "TaxiDFS.cpp"
#include "TaxiDijkstra.cpp"
#include "TaxiManual.cpp"
#include "Simulacion.cpp"
#include "MapaWidget.cpp"
#include "BarrasGananciasWidget.cpp"
#include "GraficoGananciasWidget.cpp"
#include "DialogoNodo.cpp"
#include "MainWindow.cpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("TaxiGraph"));
    app.setStyle(QStringLiteral("Fusion"));
    app.setStyleSheet(QStringLiteral(R"(
* { font-family: "Segoe UI", sans-serif; font-size: 10pt; }
QMainWindow, QDialog, QMessageBox, QInputDialog { background: #0e141d; }
QWidget { color: #dbe3ef; }
QWidget#sidePanel { background: #0e141d; }
QGroupBox {
    background: #141c28; border: 1px solid #223048; border-radius: 10px;
    margin-top: 14px; padding: 10px 8px 8px 8px; font-weight: 600;
}
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; color: #ffd60a; }
QLabel { background: transparent; }
QPushButton {
    background: #1d2a3d; border: 1px solid #2c3d58; border-radius: 8px;
    padding: 7px 12px; font-weight: 600;
}
QPushButton:hover { background: #26374f; border-color: #ffd60a; }
QPushButton:checked { background: #ffd60a; color: #10151e; border-color: #ffd60a; }
QComboBox {
    background: #1b2638; border: 1px solid #2c3d58; border-radius: 8px; padding: 5px 10px;
}
QComboBox:hover { border-color: #ffd60a; }
QComboBox QAbstractItemView {
    background: #1b2638; border: 1px solid #2c3d58; selection-background-color: #ffd60a;
    selection-color: #10151e;
}
QTabWidget::pane { border: 1px solid #223048; border-radius: 8px; background: #101722; top: -1px; }
QTabBar::tab {
    background: #141c28; border: 1px solid #223048; padding: 7px 12px;
    border-top-left-radius: 8px; border-top-right-radius: 8px; margin-right: 2px; color: #93a4bd;
}
QTabBar::tab:selected { background: #101722; color: #ffd60a; border-bottom-color: #101722; }
QTableWidget {
    background: #101722; alternate-background-color: #141d2b; gridline-color: #1c2739;
    border: none; selection-background-color: #26374f;
}
QHeaderView::section {
    background: #172233; color: #ffd60a; border: none; padding: 5px; font-weight: 600;
}
QTextEdit, QTextBrowser {
    background: #0c121b; border: 1px solid #1c2739; border-radius: 8px; color: #c8d3e4;
}
QSplitter::handle { background: #0b1018; width: 5px; }
QStatusBar { background: #0b1018; color: #93a4bd; }
)"));

    MainWindow ventana;
    ventana.show();
    return app.exec();
}
