#include "interfaz/DialogoNodo.h"

#include "ui_nodedialog.h"

#include <QColor>
#include <QDialogButtonBox>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QWidget>

void aplicarSombraControlDialogo(QWidget *widget, int desenfoque, int opacidad)
{
    if (widget == nullptr)
        return;
    QGraphicsDropShadowEffect *sombra = new QGraphicsDropShadowEffect(widget);
    sombra->setBlurRadius(desenfoque);
    sombra->setOffset(0, 2);
    sombra->setColor(QColor(0, 0, 0, opacidad));
    widget->setGraphicsEffect(sombra);
}

DialogoNodo::DialogoNodo(QWidget *parent) : QDialog(parent)
{
    ui = new Ui::NodeDialog();
    ui->setupUi(this);
    QPushButton *btnCrear = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *btnCancelar = ui->buttonBox->button(QDialogButtonBox::Cancel);
    btnCrear->setText(QStringLiteral("Crear"));
    btnCancelar->setText(QStringLiteral("Cancelar"));

    aplicarSombraControlDialogo(ui->edtName, 10, 95);
    aplicarSombraControlDialogo(ui->comboType, 10, 95);
    aplicarSombraControlDialogo(btnCrear, 12, 110);
    aplicarSombraControlDialogo(btnCancelar, 10, 95);
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
