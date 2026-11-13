#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QString>
#include <QWidget>

#include "ui_nodedialog.h"

class DialogoNodo : public QDialog
{
private:
    Ui::NodeDialog *ui;

public:
    explicit DialogoNodo(QWidget *parent = nullptr) : QDialog(parent)
    {
        ui = new Ui::NodeDialog();
        ui->setupUi(this);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Crear"));
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));
    }

    ~DialogoNodo() override
    {
        delete ui;
        ui = nullptr;
    }

    QString getNombreNodo() const
    {
        return ui->edtName->text().trimmed();
    }

    QString getTipoNodo() const
    {
        int idx = ui->comboType->currentIndex();
        if (idx == 1) return "Aeropuerto";
        if (idx == 2) return "Terminal";
        if (idx == 3) return "Turistico";
        return "Normal";
    }
};
