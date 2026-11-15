/********************************************************************************
** Form generated from reading UI file 'nodedialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NODEDIALOG_H
#define UI_NODEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NodeDialog
{
public:
    QVBoxLayout *layoutDialog;
    QFormLayout *formLayout;
    QLabel *lblName;
    QLineEdit *edtName;
    QLabel *lblType;
    QComboBox *comboType;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NodeDialog)
    {
        if (NodeDialog->objectName().isEmpty())
            NodeDialog->setObjectName("NodeDialog");
        NodeDialog->resize(380, 160);
        layoutDialog = new QVBoxLayout(NodeDialog);
        layoutDialog->setObjectName("layoutDialog");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        lblName = new QLabel(NodeDialog);
        lblName->setObjectName("lblName");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, lblName);

        edtName = new QLineEdit(NodeDialog);
        edtName->setObjectName("edtName");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, edtName);

        lblType = new QLabel(NodeDialog);
        lblType->setObjectName("lblType");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, lblType);

        comboType = new QComboBox(NodeDialog);
        comboType->addItem(QString());
        comboType->addItem(QString());
        comboType->addItem(QString());
        comboType->addItem(QString());
        comboType->setObjectName("comboType");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, comboType);


        layoutDialog->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(NodeDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        layoutDialog->addWidget(buttonBox);


        retranslateUi(NodeDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, NodeDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, NodeDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(NodeDialog);
    } // setupUi

    void retranslateUi(QDialog *NodeDialog)
    {
        NodeDialog->setWindowTitle(QCoreApplication::translate("NodeDialog", "Nuevo nodo", nullptr));
        lblName->setText(QCoreApplication::translate("NodeDialog", "Nombre del lugar", nullptr));
        edtName->setText(QCoreApplication::translate("NodeDialog", "Nuevo lugar", nullptr));
        lblType->setText(QCoreApplication::translate("NodeDialog", "Tipo de lugar", nullptr));
        comboType->setItemText(0, QCoreApplication::translate("NodeDialog", "Normal", nullptr));
        comboType->setItemText(1, QCoreApplication::translate("NodeDialog", "Aeropuerto (+$8,600)", nullptr));
        comboType->setItemText(2, QCoreApplication::translate("NodeDialog", "Terminal de transportes (+$2,400)", nullptr));
        comboType->setItemText(3, QCoreApplication::translate("NodeDialog", "Sitio tur\303\255stico (+$2,000)", nullptr));

    } // retranslateUi

};

namespace Ui {
    class NodeDialog: public Ui_NodeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NODEDIALOG_H
