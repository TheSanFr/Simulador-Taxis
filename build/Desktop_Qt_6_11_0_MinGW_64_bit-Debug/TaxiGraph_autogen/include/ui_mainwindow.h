/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *layoutMain;
    QSplitter *splitter;
    QWidget *mapHost;
    QVBoxLayout *layoutMapHost;
    QWidget *sidePanel;
    QVBoxLayout *layoutSide;
    QGroupBox *groupSim;
    QGridLayout *gridSim;
    QCheckBox *chkWeights;
    QPushButton *btnPause;
    QPushButton *btnReset;
    QLabel *lblSpeed;
    QComboBox *comboSpeed;
    QLabel *lblTimeMode;
    QComboBox *comboTimeMode;
    QGroupBox *groupEdit;
    QVBoxLayout *layoutEdit;
    QComboBox *comboEditMode;
    QLabel *lblEditHint;
    QTabWidget *tabs;
    QWidget *tabEarnings;
    QVBoxLayout *layoutEarnings;
    QWidget *barsHost;
    QVBoxLayout *layoutBarsHost;
    QTableWidget *tableTaxis;
    QWidget *tabPassengers;
    QVBoxLayout *layoutPassengers;
    QTableWidget *tablePassengers;
    QWidget *tabChart;
    QVBoxLayout *layoutChart;
    QWidget *chartHost;
    QVBoxLayout *layoutChartHost;
    QWidget *tabLog;
    QVBoxLayout *layoutLog;
    QTextEdit *txtLog;
    QWidget *tabInfo;
    QVBoxLayout *layoutInfo;
    QTextBrowser *txtInfo;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1600, 940);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        layoutMain = new QHBoxLayout(centralwidget);
        layoutMain->setSpacing(0);
        layoutMain->setObjectName("layoutMain");
        layoutMain->setContentsMargins(6, 6, 6, 6);
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        splitter->setChildrenCollapsible(false);
        mapHost = new QWidget(splitter);
        mapHost->setObjectName("mapHost");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mapHost->sizePolicy().hasHeightForWidth());
        mapHost->setSizePolicy(sizePolicy);
        layoutMapHost = new QVBoxLayout(mapHost);
        layoutMapHost->setSpacing(0);
        layoutMapHost->setObjectName("layoutMapHost");
        layoutMapHost->setContentsMargins(0, 0, 0, 0);
        splitter->addWidget(mapHost);
        sidePanel = new QWidget(splitter);
        sidePanel->setObjectName("sidePanel");
        sidePanel->setMinimumSize(QSize(440, 0));
        layoutSide = new QVBoxLayout(sidePanel);
        layoutSide->setSpacing(8);
        layoutSide->setObjectName("layoutSide");
        layoutSide->setContentsMargins(8, 0, 0, 0);
        groupSim = new QGroupBox(sidePanel);
        groupSim->setObjectName("groupSim");
        gridSim = new QGridLayout(groupSim);
        gridSim->setObjectName("gridSim");
        chkWeights = new QCheckBox(groupSim);
        chkWeights->setObjectName("chkWeights");
        chkWeights->setChecked(true);

        gridSim->addWidget(chkWeights, 3, 0, 1, 2);

        btnPause = new QPushButton(groupSim);
        btnPause->setObjectName("btnPause");
        btnPause->setCheckable(true);

        gridSim->addWidget(btnPause, 0, 0, 1, 1);

        btnReset = new QPushButton(groupSim);
        btnReset->setObjectName("btnReset");

        gridSim->addWidget(btnReset, 0, 1, 1, 1);

        lblSpeed = new QLabel(groupSim);
        lblSpeed->setObjectName("lblSpeed");

        gridSim->addWidget(lblSpeed, 1, 0, 1, 1);

        comboSpeed = new QComboBox(groupSim);
        comboSpeed->addItem(QString());
        comboSpeed->addItem(QString());
        comboSpeed->addItem(QString());
        comboSpeed->setObjectName("comboSpeed");

        gridSim->addWidget(comboSpeed, 1, 1, 1, 1);

        lblTimeMode = new QLabel(groupSim);
        lblTimeMode->setObjectName("lblTimeMode");

        gridSim->addWidget(lblTimeMode, 2, 0, 1, 1);

        comboTimeMode = new QComboBox(groupSim);
        comboTimeMode->addItem(QString());
        comboTimeMode->addItem(QString());
        comboTimeMode->addItem(QString());
        comboTimeMode->addItem(QString());
        comboTimeMode->setObjectName("comboTimeMode");

        gridSim->addWidget(comboTimeMode, 2, 1, 1, 1);


        layoutSide->addWidget(groupSim);

        groupEdit = new QGroupBox(sidePanel);
        groupEdit->setObjectName("groupEdit");
        layoutEdit = new QVBoxLayout(groupEdit);
        layoutEdit->setObjectName("layoutEdit");
        comboEditMode = new QComboBox(groupEdit);
        comboEditMode->addItem(QString());
        comboEditMode->addItem(QString());
        comboEditMode->addItem(QString());
        comboEditMode->addItem(QString());
        comboEditMode->addItem(QString());
        comboEditMode->addItem(QString());
        comboEditMode->setObjectName("comboEditMode");

        layoutEdit->addWidget(comboEditMode);

        lblEditHint = new QLabel(groupEdit);
        lblEditHint->setObjectName("lblEditHint");
        lblEditHint->setWordWrap(true);

        layoutEdit->addWidget(lblEditHint);


        layoutSide->addWidget(groupEdit);

        tabs = new QTabWidget(sidePanel);
        tabs->setObjectName("tabs");
        tabEarnings = new QWidget();
        tabEarnings->setObjectName("tabEarnings");
        layoutEarnings = new QVBoxLayout(tabEarnings);
        layoutEarnings->setObjectName("layoutEarnings");
        barsHost = new QWidget(tabEarnings);
        barsHost->setObjectName("barsHost");
        layoutBarsHost = new QVBoxLayout(barsHost);
        layoutBarsHost->setObjectName("layoutBarsHost");
        layoutBarsHost->setContentsMargins(0, 0, 0, 0);

        layoutEarnings->addWidget(barsHost);

        tableTaxis = new QTableWidget(tabEarnings);
        tableTaxis->setObjectName("tableTaxis");
        tableTaxis->setMinimumSize(QSize(0, 210));

        layoutEarnings->addWidget(tableTaxis);

        tabs->addTab(tabEarnings, QString());
        tabPassengers = new QWidget();
        tabPassengers->setObjectName("tabPassengers");
        layoutPassengers = new QVBoxLayout(tabPassengers);
        layoutPassengers->setObjectName("layoutPassengers");
        tablePassengers = new QTableWidget(tabPassengers);
        tablePassengers->setObjectName("tablePassengers");

        layoutPassengers->addWidget(tablePassengers);

        tabs->addTab(tabPassengers, QString());
        tabChart = new QWidget();
        tabChart->setObjectName("tabChart");
        layoutChart = new QVBoxLayout(tabChart);
        layoutChart->setObjectName("layoutChart");
        chartHost = new QWidget(tabChart);
        chartHost->setObjectName("chartHost");
        layoutChartHost = new QVBoxLayout(chartHost);
        layoutChartHost->setObjectName("layoutChartHost");
        layoutChartHost->setContentsMargins(0, 0, 0, 0);

        layoutChart->addWidget(chartHost);

        tabs->addTab(tabChart, QString());
        tabLog = new QWidget();
        tabLog->setObjectName("tabLog");
        layoutLog = new QVBoxLayout(tabLog);
        layoutLog->setObjectName("layoutLog");
        txtLog = new QTextEdit(tabLog);
        txtLog->setObjectName("txtLog");
        txtLog->setReadOnly(true);

        layoutLog->addWidget(txtLog);

        tabs->addTab(tabLog, QString());
        tabInfo = new QWidget();
        tabInfo->setObjectName("tabInfo");
        layoutInfo = new QVBoxLayout(tabInfo);
        layoutInfo->setObjectName("layoutInfo");
        txtInfo = new QTextBrowser(tabInfo);
        txtInfo->setObjectName("txtInfo");

        layoutInfo->addWidget(txtInfo);

        tabs->addTab(tabInfo, QString());

        layoutSide->addWidget(tabs);

        splitter->addWidget(sidePanel);

        layoutMain->addWidget(splitter);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "TaxiGraph - Simulador de taxis sobre un grafo dirigido y ponderado", nullptr));
        groupSim->setTitle(QCoreApplication::translate("MainWindow", "Simulaci\303\263n", nullptr));
        chkWeights->setText(QCoreApplication::translate("MainWindow", "Mostrar pesos de las calles", nullptr));
        btnPause->setText(QCoreApplication::translate("MainWindow", "Pausar", nullptr));
        btnReset->setText(QCoreApplication::translate("MainWindow", "Reiniciar", nullptr));
        lblSpeed->setText(QCoreApplication::translate("MainWindow", "Velocidad", nullptr));
        comboSpeed->setItemText(0, QCoreApplication::translate("MainWindow", "1x", nullptr));
        comboSpeed->setItemText(1, QCoreApplication::translate("MainWindow", "2x", nullptr));
        comboSpeed->setItemText(2, QCoreApplication::translate("MainWindow", "4x", nullptr));

        lblTimeMode->setText(QCoreApplication::translate("MainWindow", "Modo del d\303\255a", nullptr));
        comboTimeMode->setItemText(0, QCoreApplication::translate("MainWindow", "Autom\303\241tico (seg\303\272n el reloj)", nullptr));
        comboTimeMode->setItemText(1, QCoreApplication::translate("MainWindow", "Normal", nullptr));
        comboTimeMode->setItemText(2, QCoreApplication::translate("MainWindow", "Nocturno (+$2,000)", nullptr));
        comboTimeMode->setItemText(3, QCoreApplication::translate("MainWindow", "Hora pico (pesos x1.8)", nullptr));

        groupEdit->setTitle(QCoreApplication::translate("MainWindow", "Mapa y edici\303\263n del grafo", nullptr));
        comboEditMode->setItemText(0, QCoreApplication::translate("MainWindow", "Conducir mi taxi", nullptr));
        comboEditMode->setItemText(1, QCoreApplication::translate("MainWindow", "Agregar nodo", nullptr));
        comboEditMode->setItemText(2, QCoreApplication::translate("MainWindow", "Agregar calle", nullptr));
        comboEditMode->setItemText(3, QCoreApplication::translate("MainWindow", "Eliminar calle", nullptr));
        comboEditMode->setItemText(4, QCoreApplication::translate("MainWindow", "Eliminar nodo", nullptr));
        comboEditMode->setItemText(5, QCoreApplication::translate("MainWindow", "Cambiar peso de una calle", nullptr));

        lblEditHint->setText(QString());
        tabs->setTabText(tabs->indexOf(tabEarnings), QCoreApplication::translate("MainWindow", "Ganancias", nullptr));
        tabs->setTabText(tabs->indexOf(tabPassengers), QCoreApplication::translate("MainWindow", "Pasajeros", nullptr));
        tabs->setTabText(tabs->indexOf(tabChart), QCoreApplication::translate("MainWindow", "Gr\303\241fico", nullptr));
        tabs->setTabText(tabs->indexOf(tabLog), QCoreApplication::translate("MainWindow", "Eventos", nullptr));
        tabs->setTabText(tabs->indexOf(tabInfo), QCoreApplication::translate("MainWindow", "Gu\303\255a", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
