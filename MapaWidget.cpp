#include "MapaWidget.h"

#include "Arista.h"
#include "ElementoArista.h"
#include "ElementoNodo.h"
#include "Grafo.h"
#include "ListaAristas.h"
#include "ListaNodos.h"
#include "ManzanaCiudad.h"
#include "Nodo.h"
#include "Pasajero.h"
#include "RelojSimulacion.h"
#include "Simulacion.h"
#include "Taxi.h"
#include "TaxiManual.h"

#include <QEasingCurve>
#include <QEvent>
#include <QFontMetricsF>
#include <QLineF>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPolygonF>
#include <QRectF>
#include <QtMath>
#include <cmath>

namespace
{
const QColor fondoVista("#101821");
const QColor fondoPanel("#17222e");
const QColor bordePanel("#394a5a");
const QColor textoPrincipal("#e8eef4");
const QColor textoSecundario("#a5b4c3");
const QColor colorManual("#ffd166");
const QColor colorSeleccion("#70d5cc");
const QColor colorTrafico("#fb7185");
}

QFont MapaWidget::fuentePixel(int px, bool negrita)
{
    QFont f;
    f.setFamilies({QStringLiteral("Bahnschrift"), QStringLiteral("Segoe UI")});
    f.setPixelSize(px);
    f.setBold(negrita);
    return f;
}

QColor MapaWidget::obtenerColorTipoNodo(QString tipo)
{
    if (tipo == "Aeropuerto")
    {
        return QColor("#79bdff");
    }
    if (tipo == "Terminal")
    {
        return QColor("#c3b1f6");
    }
    if (tipo == "Turistico")
    {
        return QColor("#f0c77f");
    }
    return QColor("#dce8f2");
}

QColor MapaWidget::mezclarColores(QColor a, QColor b, double t)
{
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    return QColor::fromRgbF(a.redF() + (b.redF() - a.redF()) * t,
                            a.greenF() + (b.greenF() - a.greenF()) * t,
                            a.blueF() + (b.blueF() - a.blueF()) * t);
}

void MapaWidget::dibujarTextoConBorde(QPainter &p, QPointF centro, QString texto,
                                      QFont fuente, QColor relleno, double grosorBorde)
{
    QFontMetricsF fm(fuente);
    QPainterPath rutaTexto;
    rutaTexto.addText(centro.x() - fm.horizontalAdvance(texto) / 2.0, centro.y(), fuente, texto);
    p.setPen(QPen(QColor(8, 12, 20, 235), grosorBorde, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    p.drawPath(rutaTexto);
    p.setPen(Qt::NoPen);
    p.setBrush(relleno);
    p.drawPath(rutaTexto);
}

QTransform MapaWidget::transformacionVista() const
{
    QRectF b = sim->getLimitesMapa();
    double margen = 8.0;
    double escala = qMin((width() - 2 * margen) / b.width(),
                         (height() - 2 * margen) / b.height());
    QTransform t;
    t.translate((width() - b.width() * escala) / 2.0, (height() - b.height() * escala) / 2.0);
    t.scale(escala, escala);
    return t;
}

QPointF MapaWidget::posicionDibujoTaxi(const Taxi *taxi) const
{
    double rad = qDegreesToRadians(taxi->getAngulo());
    return taxi->getPosicion() + QPointF(-std::sin(rad), std::cos(rad)) * 6.0;
}

void MapaWidget::dibujarMundo(QPainter &p, bool esMinimapa)
{
    dibujarEscenario(p);
    dibujarCalles(p, esMinimapa);
    if (!esMinimapa)
    {
        dibujarRutas(p);
        if (mostrarPesos)
        {
            dibujarPesos(p);
        }
    }
    dibujarNodos(p, esMinimapa);
    dibujarPasajeros(p, esMinimapa);
    dibujarTaxis(p, esMinimapa);
    dibujarEfectosNocturnos(p);

    if (!esMinimapa)
    {
        for (int i = 0; i < textosFlotantes.size(); i++)
        {
            const TextoFlotante &tf = textosFlotantes[i];
            double k = tf.getEdad() / 2.2;
            QColor c = tf.getColor().lighter(120);
            const double avance = QEasingCurve(QEasingCurve::OutCubic).valueForProgress(k);
            const double entrada = qBound(0.0, tf.getEdad() / 0.12, 1.0);
            const double salida = qBound(0.0, (1.0 - k) / 0.42, 1.0);
            c.setAlphaF(entrada * salida);
            dibujarTextoConBorde(p, tf.getPosicion() + QPointF(0, -40 - 36 * avance),
                                 tf.getTexto(), fuentePixel(24, true), c, 3.5);
        }
    }
}

void MapaWidget::dibujarEscenario(QPainter &p)
{
    QRectF b = sim->getLimitesMapa();
    QLinearGradient fondo(b.topLeft(), b.bottomRight());
    fondo.setColorAt(0.0, QColor("#1c2b3b"));
    fondo.setColorAt(1.0, QColor("#142330"));
    p.fillRect(b, fondo);
    p.setPen(QPen(QColor(162, 184, 202, 14), 1));
    for (double x = b.left() + 48.0; x < b.right(); x += 48.0)
    {
        p.drawLine(QPointF(x, b.top()), QPointF(x, b.bottom()));
    }
    for (double y = b.top() + 48.0; y < b.bottom(); y += 48.0)
    {
        p.drawLine(QPointF(b.left(), y), QPointF(b.right(), y));
    }

    p.setPen(QPen(QColor("#2e5970"), 3));
    p.setBrush(QColor("#113246"));
    p.drawPolygon(sim->getPoligonoAgua());

    const QVector<ManzanaCiudad> &manzanas = sim->getManzanas();
    for (int i = 0; i < manzanas.size(); i++)
    {
        const ManzanaCiudad &m = manzanas[i];
        QColor color("#243546");
        if (m.getTipo() == "Parque")
        {
            color = QColor("#234637");
        }
        else if (m.getTipo() == "Industrial")
        {
            color = QColor("#424039");
        }
        else if (m.getTipo() == "Aeropuerto")
        {
            color = QColor("#293b50");
        }
        else if (m.getTipo() == "Terminal")
        {
            color = QColor("#39364c");
        }

        p.setPen(QPen(color.lighter(125), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(color);
        p.drawPolygon(m.getPoligono());
    }
}

void MapaWidget::dibujarCalles(QPainter &p, bool esMinimapa)
{
    Grafo *g = sim->getGrafo();

    ElementoArista *elem = g->getAristas()->getPrimero();
    while (elem != nullptr)
    {
        Arista *arista = elem->getArista();
        elem = elem->getSiguiente();
        if (arista == nullptr)
        {
            continue;
        }

        bool tieneInversa = g->existeAristaDirigida(arista->getDestino(), arista->getOrigen());
        if (tieneInversa && arista->getOrigen() > arista->getDestino())
        {
            continue;
        }

        Nodo *na = g->buscarNodo(arista->getOrigen());
        Nodo *nb = g->buscarNodo(arista->getDestino());
        if (na == nullptr || nb == nullptr)
        {
            continue;
        }

        bool av = arista->getEsAvenida();
        p.setPen(QPen(QColor("#101821"), av ? 22 : 16, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(na->getPosicion(), nb->getPosicion());
    }

    elem = g->getAristas()->getPrimero();
    while (elem != nullptr)
    {
        Arista *arista = elem->getArista();
        elem = elem->getSiguiente();
        if (arista == nullptr)
        {
            continue;
        }

        bool tieneInversa = g->existeAristaDirigida(arista->getDestino(), arista->getOrigen());
        if (tieneInversa && arista->getOrigen() > arista->getDestino())
        {
            continue;
        }

        Nodo *na = g->buscarNodo(arista->getOrigen());
        Nodo *nb = g->buscarNodo(arista->getDestino());
        if (na == nullptr || nb == nullptr)
        {
            continue;
        }

        bool av = arista->getEsAvenida();
        QColor base = av ? QColor("#a2b8ca") : QColor("#73899e");
        QColor c = mezclarColores(base, colorTrafico,
                                  qBound(0.0, (g->obtenerNivelCongestion(arista) - 1.05) * 0.30, 0.58));
        p.setPen(QPen(c, av ? 16 : 10, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(na->getPosicion(), nb->getPosicion());
    }

    if (esMinimapa)
    {
        return;
    }

    elem = g->getAristas()->getPrimero();
    while (elem != nullptr)
    {
        Arista *arista = elem->getArista();
        elem = elem->getSiguiente();
        if (arista == nullptr)
        {
            continue;
        }

        bool tieneInversa = g->existeAristaDirigida(arista->getDestino(), arista->getOrigen());
        if (tieneInversa && arista->getOrigen() > arista->getDestino())
        {
            continue;
        }

        Nodo *na = g->buscarNodo(arista->getOrigen());
        Nodo *nb = g->buscarNodo(arista->getDestino());
        if (na == nullptr || nb == nullptr)
        {
            continue;
        }

        QPointF a = na->getPosicion();
        QPointF b = nb->getPosicion();
        const QLineF linea(a, b);
        if (linea.length() < 34.0)
        {
            continue;
        }

        const QPointF sentido = (b - a) / linea.length();
        p.setPen(QPen(QColor("#142430"), 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto chevron = [&p](QPointF centro, QPointF direccion) {
            const QPointF lado(-direccion.y(), direccion.x());
            const QPointF puntos[] = {
                centro - direccion * 4.5 + lado * 3.2,
                centro + direccion * 4.5,
                centro - direccion * 4.5 - lado * 3.2
            };
            p.drawPolyline(puntos, 3);
        };
        chevron(linea.pointAt(0.64), sentido);
        if (tieneInversa)
        {
            chevron(linea.pointAt(0.36), -sentido);
        }
    }
}

void MapaWidget::dibujarRutas(QPainter &p)
{
    Grafo *g = sim->getGrafo();
    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        QPolygonF poli;
        poli << taxi->getPosicion();

        if (taxi->getEnMovimiento() && g->buscarNodo(taxi->getNodoSiguiente()) != nullptr)
        {
            poli << g->buscarNodo(taxi->getNodoSiguiente())->getPosicion();
        }

        for (int k = 0; k < taxi->getCantidadRuta(); k++)
        {
            Nodo *n = g->buscarNodo(taxi->getPasoRuta(k));
            if (n != nullptr)
            {
                poli << n->getPosicion();
            }
        }

        if (poli.size() < 2)
        {
            continue;
        }

        QColor c = taxi->esManual() ? colorManual : taxi->getColor();
        c.setAlpha(taxi->esManual() ? 255 : 180);
        p.setPen(QPen(c, taxi->esManual() ? 6 : 3.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        p.drawPolyline(poli);
    }
}

void MapaWidget::dibujarPesos(QPainter &p)
{
    Grafo *g = sim->getGrafo();
    QFont fuente = fuentePixel(12, true);
    QFontMetricsF fm(fuente);
    p.setFont(fuente);

    ElementoArista *elem = g->getAristas()->getPrimero();
    while (elem != nullptr)
    {
        Arista *a = elem->getArista();
        elem = elem->getSiguiente();
        if (a == nullptr)
        {
            continue;
        }

        if (a->getOrigen() > a->getDestino()
            && g->existeAristaDirigida(a->getDestino(), a->getOrigen()))
        {
            continue;
        }

        Nodo *n1 = g->buscarNodo(a->getOrigen());
        Nodo *n2 = g->buscarNodo(a->getDestino());
        if (n1 == nullptr || n2 == nullptr)
        {
            continue;
        }

        QPointF centro = (n1->getPosicion() + n2->getPosicion()) / 2.0;
        QString texto = QString::number(g->obtenerPesoEfectivoArista(a), 'f', 1);
        QColor tono = mezclarColores(textoPrincipal, colorTrafico,
                                     (g->obtenerNivelCongestion(a) - 1.05) / 1.6);
        double ancho = fm.horizontalAdvance(texto) + 10;
        QRectF caja(centro.x() - ancho / 2, centro.y() - 8, ancho, 16);
        p.setPen(QPen(bordePanel, 1.5));
        p.setBrush(fondoPanel);
        p.drawRoundedRect(caja, 4, 4);
        p.setPen(tono);
        p.drawText(caja, Qt::AlignCenter, texto);
    }
}

void MapaWidget::dibujarNodos(QPainter &p, bool esMinimapa)
{
    Grafo *g = sim->getGrafo();

    ElementoNodo *elem = g->getNodos()->getPrimero();
    while (elem != nullptr)
    {
        Nodo *n = elem->getNodo();
        elem = elem->getSiguiente();
        if (n == nullptr)
        {
            continue;
        }

        QColor colorTipo = obtenerColorTipoNodo(n->getTipo());
        bool especial = (n->getTipo() != "Normal");
        double radio = especial ? (esMinimapa ? 13 : 16) : 8.5;

        if (!esMinimapa && n->getId() == nodoBajoMouse)
        {
            const double avance = animacionNodo.currentValue().toDouble();
            QColor halo = colorSeleccion;
            halo.setAlphaF(0.68 * avance);
            p.setPen(QPen(halo, 2.5));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(n->getPosicion(), radio + 7.0 * avance, radio + 7.0 * avance);
        }

        p.setPen(QPen(fondoVista, especial ? 3.2 : 2.6));
        p.setBrush(colorTipo);
        p.drawEllipse(n->getPosicion(), radio, radio);

        if (especial)
        {
            p.save();
            p.translate(n->getPosicion());
            p.setPen(QPen(fondoVista, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(fondoVista);

            if (n->getTipo() == "Aeropuerto")
            {
                p.drawLine(QPointF(0, -8), QPointF(0, 8));
                const QPointF alas[] = {QPointF(-8, 2), QPointF(0, -2), QPointF(8, 2)};
                const QPointF cola[] = {QPointF(-4, 7), QPointF(0, 5), QPointF(4, 7)};
                p.drawPolyline(alas, 3);
                p.drawPolyline(cola, 3);
            }
            else if (n->getTipo() == "Terminal")
            {
                p.drawRoundedRect(QRectF(-7, -7, 14, 13), 3, 3);
                p.setPen(QPen(colorTipo, 2));
                p.drawLine(QPointF(-3, -2), QPointF(3, -2));
                p.drawLine(QPointF(-3, 2), QPointF(3, 2));
                p.setPen(QPen(fondoVista, 2.2, Qt::SolidLine, Qt::RoundCap));
                p.drawLine(QPointF(-4, 6), QPointF(-4, 8));
                p.drawLine(QPointF(4, 6), QPointF(4, 8));
            }
            else
            {
                p.setPen(Qt::NoPen);
                QPolygonF estrella;
                for (int i = 0; i < 10; ++i)
                {
                    const double angulo = qDegreesToRadians(i * 36.0 - 90.0);
                    const double r = i % 2 == 0 ? 9.0 : 4.4;
                    estrella << QPointF(r * std::cos(angulo), r * std::sin(angulo));
                }
                p.drawPolygon(estrella);
            }
            p.restore();
        }

        if (!esMinimapa)
        {
            bool resaltado = (n->getId() == nodoBajoMouse);
            const double escala = p.worldTransform().m11();
            const int px = qBound(especial ? 16 : 14, qRound(9.0 / escala), 24);
            const QFont f = fuentePixel(px, especial || resaltado);
            const QColor c = resaltado ? QColor(Qt::white) : (especial ? colorTipo : textoPrincipal);
            const QString nombre = resaltado ? n->getNombre()
                                            : QFontMetricsF(f).elidedText(n->getNombre(), Qt::ElideRight, 220);
            dibujarTextoConBorde(p, n->getPosicion() + QPointF(0, especial ? 35 : 27),
                                 nombre, f, c);
        }
    }

    if (esMinimapa)
    {
        return;
    }

    if (g->buscarNodo(nodoPendiente) != nullptr)
    {
        p.setPen(QPen(colorSeleccion, 3.5));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(g->buscarNodo(nodoPendiente)->getPosicion(), 25, 25);
    }

    if (modoConducir && sim->getTaxiManual() != nullptr)
    {
        constexpr double r = 23.0;
        p.setPen(QPen(colorManual, 2.6, Qt::DashLine));
        QColor relleno = colorManual;
        relleno.setAlpha(36);
        p.setBrush(relleno);

        int desde = sim->getTaxiManual()->nodoFinalDeRuta();
        ElementoArista *ea = g->getAristas()->getPrimero();
        while (ea != nullptr)
        {
            Arista *a = ea->getArista();
            if (a != nullptr && a->getOrigen() == desde)
            {
                Nodo *vecino = g->buscarNodo(a->getDestino());
                if (vecino != nullptr)
                {
                    p.drawEllipse(vecino->getPosicion(), r, r);
                }
            }
            ea = ea->getSiguiente();
        }
    }
}

void MapaWidget::dibujarPasajeros(QPainter &p, bool esMinimapa)
{
    Grafo *g = sim->getGrafo();
    const QVector<Pasajero> &lista = sim->getPasajerosEnEspera();

    for (int i = 0; i < lista.size(); i++)
    {
        const Pasajero &pas = lista[i];
        Nodo *no = g->buscarNodo(pas.getOrigen());
        Nodo *nd = g->buscarNodo(pas.getDestino());
        if (no == nullptr || nd == nullptr)
        {
            continue;
        }

        QPointF o = no->getPosicion();
        bool libre = !pas.estaReservado();
        QColor colorAro = colorSeleccion;

        if (!libre)
        {
            for (int k = 0; k < sim->getCantidadTaxis(); k++)
            {
                Taxi *tx = sim->getTaxi(k);
                if (tx->getId() == pas.getIdTaxiAsignado())
                {
                    colorAro = tx->esManual() ? colorManual : tx->getColor();
                }
            }
        }

        double r = libre ? 21.0 : 17.0;
        p.setPen(QPen(colorAro, libre ? 2.6 : 1.8));
        p.setBrush(QColor(colorAro.red(), colorAro.green(), colorAro.blue(), libre ? 32 : 20));
        p.drawEllipse(o, r, r);

        p.save();
        p.translate(o + QPointF(0, -26));
        p.setPen(QPen(QColor(8, 12, 20), 2));
        p.setBrush(colorAro);
        p.drawEllipse(QPointF(0, -6), 5.0, 5.0);
        p.drawRoundedRect(QRectF(-6, 0, 12, 10), 4, 4);
        p.restore();

        if (libre && !esMinimapa && no->getId() == nodoBajoMouse)
        {
            QString texto = QStringLiteral("Pasajero → %1  ·  %2")
                                .arg(nd->getNombre(),
                                     Grafo::formatearPesos(sim->cotizarCarrera(pas.getOrigen(), pas.getDestino())));
            QFont f = fuentePixel(12, true);
            QFontMetricsF fm(f);
            double w = fm.horizontalAdvance(texto) + 14;
            QRectF globo(o.x() - w / 2, o.y() - 61, w, 20);
            p.setPen(QPen(colorAro, 1.8));
            p.setBrush(fondoPanel);
            p.drawRoundedRect(globo, 7, 7);
            p.setFont(f);
            p.setPen(QColor(255, 255, 255));
            p.drawText(globo, Qt::AlignCenter, texto);
        }
    }

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        if (!taxi->getLlevaPasajero())
        {
            continue;
        }
        Nodo *nd = g->buscarNodo(taxi->getDestinoPasajeroABordo());
        if (nd == nullptr)
        {
            continue;
        }
        QPointF d = nd->getPosicion();
        p.save();
        p.translate(d + QPointF(0, -34));
        QPolygonF rombo;
        rombo << QPointF(0, -11) << QPointF(9, 0) << QPointF(0, 11) << QPointF(-9, 0);
        p.setPen(QPen(QColor(8, 12, 20), 2.5));
        p.setBrush(taxi->esManual() ? colorManual : taxi->getColor());
        p.drawPolygon(rombo);
        p.restore();

        if (taxi->esManual() && !esMinimapa)
        {
            dibujarTextoConBorde(p, d + QPointF(0, -54), QStringLiteral("DESTINO"),
                                 fuentePixel(14, true), colorManual);
        }
    }
}

void MapaWidget::dibujarTaxis(QPainter &p, bool esMinimapa)
{
    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        QPointF pos = posicionDibujoTaxi(taxi);

        p.save();
        p.translate(pos);
        p.rotate(taxi->getAngulo());
        const QColor color = taxi->esManual() ? colorManual : taxi->getColor();
        if (!esMinimapa)
        {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(3, 8, 14, 125));
            p.drawRoundedRect(QRectF(-16, -6, 35, 19), 5, 5);
        }
        p.setPen(QPen(fondoVista, 2.4));
        p.setBrush(color);
        p.drawRoundedRect(QRectF(-16, -9, 32, 18), 5, 5);
        p.setPen(Qt::NoPen);
        p.setBrush(fondoVista);
        p.drawRoundedRect(QRectF(3, -6, 6, 12), 2, 2);
        p.drawRoundedRect(QRectF(-11, -6, 4, 12), 2, 2);
        p.setBrush(color.lighter(150));
        p.drawRoundedRect(QRectF(12, -6.5, 3, 4), 1, 1);
        p.drawRoundedRect(QRectF(12, 2.5, 3, 4), 1, 1);
        p.restore();

        if (esMinimapa)
        {
            continue;
        }

        QString etiqueta = taxi->getNombre();
        if (taxi->getLlevaPasajero())
        {
            etiqueta += QStringLiteral(" ●");
        }
        QFont f = fuentePixel(12, true);
        QFontMetricsF fm(f);
        double w = fm.horizontalAdvance(etiqueta) + 12;
        QRectF caja(pos.x() - w / 2, pos.y() - 36, w, 17);
        p.setPen(QPen(taxi->esManual() ? colorManual : bordePanel, 1.6));
        p.setBrush(taxi->esManual() ? colorManual : fondoPanel);
        p.drawRoundedRect(caja, 5, 5);
        p.setFont(f);
        p.setPen(taxi->esManual() ? fondoVista : textoPrincipal);
        p.drawText(caja, Qt::AlignCenter, etiqueta);
    }
}

void MapaWidget::dibujarEfectosNocturnos(QPainter &p)
{
    if (oscuridad < 0.01)
    {
        return;
    }

    p.fillRect(sim->getLimitesMapa(), QColor(6, 12, 46, static_cast<int>(96 * oscuridad)));

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        p.save();
        p.translate(posicionDibujoTaxi(taxi));
        p.rotate(taxi->getAngulo());
        QLinearGradient haz(10, 0, 70, 0);
        haz.setColorAt(0.0, QColor(255, 244, 200, static_cast<int>(170 * oscuridad)));
        haz.setColorAt(1.0, QColor(255, 244, 200, 0));
        p.setPen(Qt::NoPen);
        p.setBrush(haz);
        QPolygonF cono;
        cono << QPointF(10, -5) << QPointF(70, -28) << QPointF(70, 28) << QPointF(10, 5);
        p.drawPolygon(cono);
        p.restore();
    }
}

void MapaWidget::dibujarMinimapa(QPainter &p)
{
    TaxiManual *yo = sim->getTaxiManual();
    if (yo == nullptr || width() < 1000 || height() < 575)
    {
        return;
    }

    const QRectF panel(14, height() - 110, 138, 96);
    const QRectF vista = panel.adjusted(1, 19, -1, -1);

    p.setPen(QPen(bordePanel, 1));
    p.setBrush(fondoPanel);
    p.drawRoundedRect(panel, 8, 8);

    QPainterPath recorte;
    recorte.addRoundedRect(vista, 6, 6);
    p.save();
    p.setClipPath(recorte);
    p.fillRect(vista, fondoVista);

    QTransform t;
    t.translate(vista.center().x(), vista.center().y());
    t.scale(0.20, 0.20);
    t.translate(-yo->getPosicion().x(), -yo->getPosicion().y());
    p.setTransform(t);
    dibujarMundo(p, true);
    p.restore();

    p.setFont(fuentePixel(10, true));
    p.setPen(textoSecundario);
    p.drawText(panel.adjusted(10, 1, -10, -77), Qt::AlignVCenter | Qt::AlignLeft,
               QStringLiteral("Minimapa"));
}

void MapaWidget::enCuadro(double dt)
{
    double meta = sim->getReloj().oscuridad();
    double factor = dt * 2.5;
    if (factor > 1.0) factor = 1.0;
    oscuridad += (meta - oscuridad) * factor;

    for (int i = textosFlotantes.size() - 1; i >= 0; i--)
    {
        textosFlotantes[i].avanzarEdad(dt);
        if (textosFlotantes[i].getEdad() > 2.2)
        {
            textosFlotantes.removeAt(i);
        }
    }
    update();
}

void MapaWidget::enCarreraCompletada(int, QPointF pos, double tarifa, QColor color)
{
    textosFlotantes.append(
        TextoFlotante(pos, QStringLiteral("+") + Grafo::formatearPesos(tarifa), color));
}

void MapaWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.fillRect(rect(), QColor("#0d141b"));
    const QRectF borde = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    QPainterPath recorte;
    recorte.addRoundedRect(borde, 4, 4);
    p.setClipPath(recorte);
    p.fillRect(borde.adjusted(1, 1, -1, -1), fondoVista);
    if (sim == nullptr)
    {
        return;
    }

    p.save();
    p.setTransform(transformacionVista());
    p.setClipRect(sim->getLimitesMapa(), Qt::IntersectClip);
    dibujarMundo(p, false);
    p.restore();

    dibujarMinimapa(p);
    p.setClipping(false);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(bordePanel, 1));
    p.drawRoundedRect(borde, 4, 4);
}

void MapaWidget::mousePressEvent(QMouseEvent *e)
{
    if (sim == nullptr)
    {
        return;
    }
    if (e->button() == Qt::RightButton)
    {
        emit cancelarSolicitado();
        return;
    }
    if (e->button() != Qt::LeftButton)
    {
        return;
    }

    QPointF posMapa = transformacionVista().inverted().map(QPointF(e->pos()));
    int idNodo = sim->getGrafo()->nodoMasCercano(posMapa, 30.0);
    if (idNodo >= 0)
    {
        emit nodoClicado(idNodo);
    }
    else if (sim->getLimitesMapa().contains(posMapa))
    {
        emit espacioVacioClicado(posMapa);
    }
}

void MapaWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (sim == nullptr)
    {
        return;
    }
    QPointF posMapa = transformacionVista().inverted().map(QPointF(e->pos()));
    seleccionarNodoEnFoco(sim->getGrafo()->nodoMasCercano(posMapa, 30.0));
}

void MapaWidget::leaveEvent(QEvent *)
{
    seleccionarNodoEnFoco(-1);
}

void MapaWidget::seleccionarNodoEnFoco(int id)
{
    if (nodoBajoMouse == id)
    {
        return;
    }

    nodoBajoMouse = id;
    setCursor(id >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    animacionNodo.stop();
    if (id >= 0)
    {
        animacionNodo.start();
    }
    emit nodoEnFocoCambiado(id);
    update();
}

MapaWidget::MapaWidget(QWidget *parent) : QWidget(parent)
{
    sim = nullptr;
    oscuridad = 0.0;
    nodoBajoMouse = -1;
    nodoPendiente = -1;
    modoConducir = true;
    mostrarPesos = true;

    setMouseTracking(true);
    setMinimumSize(560, 420);
    animacionNodo.setDuration(145);
    animacionNodo.setStartValue(0.0);
    animacionNodo.setEndValue(1.0);
    animacionNodo.setEasingCurve(QEasingCurve::OutCubic);
    connect(&animacionNodo, &QVariantAnimation::valueChanged, this, [this] { update(); });
}

void MapaWidget::setSimulacion(Simulacion *nuevaSim)
{
    sim = nuevaSim;
    connect(sim, &Simulacion::cuadroActualizado, this, &MapaWidget::enCuadro);
    connect(sim, &Simulacion::carreraCompletada, this, &MapaWidget::enCarreraCompletada);
    connect(sim->getGrafo(), &Grafo::grafoModificado, this, [this] {
        if (!sim->getGrafo()->tieneNodo(nodoBajoMouse))
        {
            seleccionarNodoEnFoco(-1);
        }
        update();
    });
}

void MapaWidget::setNodoPendiente(int idNodo)
{
    nodoPendiente = idNodo;
}

void MapaWidget::setModoConducir(bool activo)
{
    modoConducir = activo;
}

void MapaWidget::setMostrarPesos(bool mostrar)
{
    mostrarPesos = mostrar;
    update();
}
