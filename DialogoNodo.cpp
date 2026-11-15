#include "DialogoNodo.h"

#include "ui_nodedialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QWidget>

DialogoNodo::DialogoNodo(QWidget *parent) : QDialog(parent)
{
    ui = new Ui::NodeDialog();
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Crear"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));
}

DialogoNodo::~DialogoNodo()
{
    delete ui;
    ui = nullptr;
}

QString DialogoNodo::getNombreNodo() const
{
    return ui->edtName->text().trimmed();
}

QString DialogoNodo::getTipoNodo() const
{
    int idx = ui->comboType->currentIndex();
    if (idx == 1) return "Aeropuerto";
    if (idx == 2) return "Terminal";
    if (idx == 3) return "Turistico";
    return "Normal";
}
