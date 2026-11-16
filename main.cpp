#include "MainWindow.h"

#include <QApplication>
#include <QColor>
#include <QFile>
#include <QFont>
#include <QPalette>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("TaxiGraph"));
    app.setStyle(QStringLiteral("Fusion"));
    app.setFont(QFont(QStringLiteral("Segoe UI"), 10));

    QPalette paleta;
    paleta.setColor(QPalette::Window, QColor("#101821"));
    paleta.setColor(QPalette::WindowText, QColor("#e8eef4"));
    paleta.setColor(QPalette::Base, QColor("#131d28"));
    paleta.setColor(QPalette::AlternateBase, QColor("#1b2835"));
    paleta.setColor(QPalette::Text, QColor("#e8eef4"));
    paleta.setColor(QPalette::Button, QColor("#202e3d"));
    paleta.setColor(QPalette::ButtonText, QColor("#e8eef4"));
    paleta.setColor(QPalette::Highlight, QColor("#38536b"));
    paleta.setColor(QPalette::HighlightedText, Qt::white);
    paleta.setColor(QPalette::Disabled, QPalette::Text, QColor("#81909f"));
    paleta.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#81909f"));
    paleta.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#81909f"));
    app.setPalette(paleta);

    QFile tema(QStringLiteral(":/interfaz.qss"));
    if (tema.open(QIODevice::ReadOnly))
    {
        app.setStyleSheet(QString::fromUtf8(tema.readAll()));
    }

    MainWindow ventana;
    ventana.show();
    return app.exec();
}
