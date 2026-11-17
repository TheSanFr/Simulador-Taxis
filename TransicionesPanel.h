#ifndef TRANSICIONES_PANEL_H
#define TRANSICIONES_PANEL_H

#include <QEasingCurve>
#include <QEvent>
#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QTabWidget>
#include <QVariantAnimation>
#include <QWidget>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

class TransicionesPanel final : public QWidget
{
public:
    explicit TransicionesPanel(QTabWidget *pestanas)
        : QWidget(pestanas->currentWidget()->parentWidget()), anterior(pestanas->currentWidget())
    {
        setObjectName(QStringLiteral("transicionPestanas"));
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setFocusPolicy(Qt::NoFocus);
        hide();
        parentWidget()->installEventFilter(this);

        animacion.setStartValue(0.0);
        animacion.setEndValue(1.0);
        animacion.setDuration(150);
        animacion.setEasingCurve(QEasingCurve::OutCubic);
        connect(&animacion, &QVariantAnimation::valueChanged, this, [this] { update(); });
        connect(&animacion, &QVariantAnimation::finished, this, [this] { finalizar(); });
        connect(pestanas, &QTabWidget::currentChanged, this, [this, pestanas](int indice) {
            QWidget *saliente = anterior;
            anterior = pestanas->widget(indice);
            const int direccion = indice >= indiceAnterior ? -1 : 1;
            indiceAnterior = indice;
            finalizar();
            if (saliente == nullptr || anterior.isNull() || !anterior->isVisible()
                || !animacionesPermitidas())
                return;

            const qreal escala = saliente->devicePixelRatioF();
            constexpr qreal maximoPixeles = 3200000.0;
            if (saliente->width() * escala * saliente->height() * escala > maximoPixeles)
                return;
            captura = saliente->grab();
            if (captura.isNull())
                return;
            desplazamiento = direccion * 14;
            setGeometry(anterior->geometry());
            raise();
            show();
            animacion.start();
        });
        indiceAnterior = pestanas->currentIndex();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        if (captura.isNull())
            return;
        const double avance = animacion.currentValue().toDouble();
        QPainter pintor(this);
        pintor.setOpacity(1.0 - avance);
        pintor.drawPixmap(QPointF(desplazamiento * avance, 0), captura);
    }

    bool eventFilter(QObject *objeto, QEvent *evento) override
    {
        if (objeto == parentWidget() && evento->type() == QEvent::Resize)
            finalizar();
        return QWidget::eventFilter(objeto, evento);
    }

private:
    static bool animacionesPermitidas()
    {
#ifdef Q_OS_WIN
        BOOL activadas = TRUE;
        if (SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &activadas, 0))
            return activadas != FALSE;
#endif
        return true;
    }

    void finalizar()
    {
        animacion.stop();
        hide();
        captura = {};
    }

    QPointer<QWidget> anterior;
    QPixmap captura;
    QVariantAnimation animacion;
    int indiceAnterior = 0;
    int desplazamiento = 0;
};

#endif
