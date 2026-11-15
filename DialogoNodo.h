#pragma once

#include <QDialog>
#include <QString>

namespace Ui
{
    class NodeDialog;
}

class DialogoNodo : public QDialog
{
private:
    Ui::NodeDialog *ui;

public:
    explicit DialogoNodo(QWidget *parent = nullptr);

    ~DialogoNodo() override;

    QString getNombreNodo() const;

    QString getTipoNodo() const;
};
