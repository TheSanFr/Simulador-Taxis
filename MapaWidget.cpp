#pragma once

#include <QColor>
#include <QElapsedTimer>
#include <QEvent>
#include <QFont>
#include <QFontMetricsF>
#include <QLineF>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>
#include <QString>
#include <QTransform>
#include <QVector>
#include <QWidget>
#include <QtMath>
#include <cmath>

#include "Arista.cpp"
#include "ElementoArista.cpp"
#include "ElementoNodo.cpp"
#include "Grafo.cpp"
#include "ManzanaCiudad.cpp"
#include "Nodo.cpp"
#include "Pasajero.cpp"
#include "RelojSimulacion.cpp"
#include "Simulacion.cpp"
#include "Taxi.cpp"
#include "TaxiManual.cpp"
#include "TextoFlotante.cpp"

class MapaWidget : public QWidget
{
    Q_OBJECT

private:
    Simulacion *sim;
    QElapsedTimer relojAnimacion;
    double oscuridad;
    int nodoBajoMouse;
    int nodoPendiente;
    bool modoConducir;
    bool mostrarPesos;
    QVector<TextoFlotante> textosFlotantes;

    static QFont fuentePixel(int px, bool negrita = false)
    {
        QFont f(QStringLiteral("Segoe UI"));
        f.setPixelSize(px);
        f.setBold(negrita);
        return f;
    }

    static QColor obtenerColorTipoNodo(QString tipo)
    {
        if (tipo == "Aeropuerto")
        {
            return QColor("#2f80ed");
        }
        if (tipo == "Terminal")
        {
            return QColor("#f2994a");
        }
        if (tipo == "Turistico")
        {
            return QColor("#f2c94c");
        }
        return QColor("#eef1f6");
    }

    static QColor mezclarColores(QColor a, QColor b, double t)
    {
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;
        return QColor::fromRgbF(a.redF() + (b.redF() - a.redF()) * t,
                                a.greenF() + (b.greenF() - a.greenF()) * t,
                                a.blueF() + (b.blueF() - a.blueF()) * t);
    }

    static void dibujarTextoConBorde(QPainter &p, QPointF centro, QString texto,
                                     QFont fuente, QColor relleno, double grosorBorde = 3.5)
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

    QTransform transformacionVista() const
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

    QPointF posicionDibujoTaxi(const Taxi *taxi) const
    {
        double rad = qDegreesToRadians(taxi->getAngulo());
        return taxi->getPosicion() + QPointF(-std::sin(rad), std::cos(rad)) * 6.0;
    }

    void dibujarMundo(QPainter &p, bool esMinimapa)
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
                double alfa = 1.0 - k * k;
                if (alfa < 0.0) alfa = 0.0;
                if (alfa > 1.0) alfa = 1.0;
                c.setAlpha(static_cast<int>(255 * alfa));
                dibujarTextoConBorde(p, tf.getPosicion() + QPointF(0, -50 - 55 * k),
                                     tf.getTexto(), fuentePixel(26, true), c, 4.5);
            }
        }
    }

    void dibujarEscenario(QPainter &p)
    {
        QRectF b = sim->getLimitesMapa();
        QLinearGradient fondo(b.topLeft(), b.bottomRight());
        fondo.setColorAt(0.0, QColor("#22304a"));
        fondo.setColorAt(1.0, QColor("#1b2639"));
        p.fillRect(b, fondo);

        p.setPen(QPen(QColor("#2d79b3"), 6));
        p.setBrush(QColor("#14395e"));
        p.drawPolygon(sim->getPoligonoAgua());

        const QVector<ManzanaCiudad> &manzanas = sim->getManzanas();
        for (int i = 0; i < manzanas.size(); i++)
        {
            const ManzanaCiudad &m = manzanas[i];
            QColor color = QColor("#2f4160");
            if (m.getTipo() == "Parque")
            {
                color = QColor("#2e6b4a");
            }
            else if (m.getTipo() == "Industrial")
            {
                color = QColor("#51483c");
            }
            else if (m.getTipo() == "Aeropuerto")
            {
                color = QColor("#3b4d6b");
            }
            else if (m.getTipo() == "Terminal")
            {
                color = QColor("#4d4363");
            }

            p.setPen(QPen(color, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(color);
            p.drawPolygon(m.getPoligono());
        }
    }

    void dibujarCalles(QPainter &p, bool esMinimapa)
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
            p.setPen(QPen(QColor("#0e141f"), av ? 25 : 19, Qt::SolidLine, Qt::RoundCap));
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
            QColor base = av ? QColor("#e9edf5") : QColor("#9fb0c7");
            QColor c = mezclarColores(base, QColor("#ff4a3d"),
                                      (g->obtenerNivelCongestion(arista) - 1.05) / 2.0);
            p.setPen(QPen(c, av ? 19 : 13, Qt::SolidLine, Qt::RoundCap));
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
            QLineF linea(a, b);
            if (tieneInversa)
            {
                p.setPen(QPen(QColor(40, 50, 68, 150), 1.6, Qt::DashLine));
                p.drawLine(linea);
            }
            else
            {
                double lon = linea.length();
                if (lon < 1.0)
                {
                    continue;
                }
                QPointF d = (b - a) / lon;
                QPointF n(-d.y(), d.x());
                QPointF c = (a + b) / 2.0;
                QPointF pts[3] = {c - d * 5.0 + n * 5.0, c + d * 5.0, c - d * 5.0 - n * 5.0};
                p.setPen(QPen(QColor(38, 48, 66), 2.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                p.drawPolyline(pts, 3);
            }
        }
    }

    void dibujarRutas(QPainter &p)
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

            QColor c = taxi->getColor();
            c.setAlpha(taxi->esManual() ? 220 : 130);
            p.setPen(QPen(c, taxi->esManual() ? 6 : 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.setBrush(Qt::NoBrush);
            p.drawPolyline(poli);
        }
    }

    void dibujarPesos(QPainter &p)
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
            QColor tono = mezclarColores(QColor(235, 240, 250), QColor("#ff4a3d"),
                                         (g->obtenerNivelCongestion(a) - 1.05) / 1.6);
            double ancho = fm.horizontalAdvance(texto) + 10;
            QRectF caja(centro.x() - ancho / 2, centro.y() - 8, ancho, 16);
            p.setPen(QPen(tono, 1.6));
            p.setBrush(QColor(10, 14, 24, 232));
            p.drawRoundedRect(caja, 6, 6);
            p.setPen(tono);
            p.drawText(caja, Qt::AlignCenter, texto);
        }
    }

    void dibujarNodos(QPainter &p, bool esMinimapa)
    {
        Grafo *g = sim->getGrafo();
        double t = relojAnimacion.elapsed() / 1000.0;

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

            p.setPen(QPen(especial ? QColor(255, 255, 255) : QColor("#0e141f"), 3.2));
            p.setBrush(colorTipo);
            p.drawEllipse(n->getPosicion(), radio, radio);

            if (especial)
            {
                QString simbolo = QStringLiteral("★");
                if (n->getTipo() == "Aeropuerto")
                {
                    simbolo = QStringLiteral("✈");
                }
                else if (n->getTipo() == "Terminal")
                {
                    simbolo = QStringLiteral("▣");
                }
                p.setFont(fuentePixel(esMinimapa ? 13 : 16, true));
                p.setPen(n->getTipo() == "Turistico" ? QColor(65, 38, 0) : QColor(255, 255, 255));
                p.drawText(QRectF(n->getPosicion().x() - radio, n->getPosicion().y() - radio,
                                  radio * 2, radio * 2),
                           Qt::AlignCenter, simbolo);
            }

            if (!esMinimapa)
            {
                bool resaltado = (n->getId() == nodoBajoMouse);
                QFont f = fuentePixel(especial ? 15 : 13, especial || resaltado);
                QColor c = resaltado ? QColor(255, 255, 255)
                                     : (especial ? colorTipo : QColor(200, 212, 230));
                dibujarTextoConBorde(p, n->getPosicion() + QPointF(0, especial ? 34 : 26),
                                     n->getNombre(), f, c);
            }
        }

        if (esMinimapa)
        {
            return;
        }

        if (g->buscarNodo(nodoPendiente) != nullptr)
        {
            p.setPen(QPen(QColor("#ff9f1c"), 4));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(g->buscarNodo(nodoPendiente)->getPosicion(),
                          26 + 3 * std::sin(t * 6), 26 + 3 * std::sin(t * 6));
        }

        if (modoConducir && sim->getTaxiManual() != nullptr)
        {
            double r = 24 + 3 * std::sin(t * 5);
            p.setPen(QPen(QColor(255, 214, 10, 230), 3, Qt::DashLine));
            p.setBrush(QColor(255, 214, 10, 40));

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

    void dibujarPasajeros(QPainter &p, bool esMinimapa)
    {
        Grafo *g = sim->getGrafo();
        double t = relojAnimacion.elapsed() / 1000.0;
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
            QColor colorAro("#5eead4");

            if (!libre)
            {
                for (int k = 0; k < sim->getCantidadTaxis(); k++)
                {
                    Taxi *tx = sim->getTaxi(k);
                    if (tx->getId() == pas.getIdTaxiAsignado())
                    {
                        colorAro = tx->getColor();
                    }
                }
            }

            double r = libre ? 26 + 4 * std::sin(t * 5 + pas.getId()) : 18;
            p.setPen(QPen(colorAro, libre ? 4.0 : 2.5));
            p.setBrush(QColor(colorAro.red(), colorAro.green(), colorAro.blue(), libre ? 70 : 30));
            p.drawEllipse(o, r, r);

            double salto = libre ? -4.0 * std::abs(std::sin(t * 3 + pas.getId())) : 0.0;
            p.save();
            p.translate(o + QPointF(0, -28 + salto));
            p.setPen(QPen(QColor(8, 12, 20), 2));
            p.setBrush(colorAro);
            p.drawEllipse(QPointF(0, -6), 5.0, 5.0);
            p.drawRoundedRect(QRectF(-6, 0, 12, 10), 4, 4);
            p.restore();

            if (libre && !esMinimapa)
            {
                QString texto = QStringLiteral("Pasajero → %1  ·  %2")
                                    .arg(nd->getNombre(),
                                         Grafo::formatearPesos(sim->cotizarCarrera(pas.getOrigen(), pas.getDestino())));
                QFont f = fuentePixel(12, true);
                QFontMetricsF fm(f);
                double w = fm.horizontalAdvance(texto) + 14;
                QRectF globo(o.x() - w / 2, o.y() - 64 + salto, w, 20);
                p.setPen(QPen(colorAro, 1.8));
                p.setBrush(QColor(8, 12, 20, 235));
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
            p.setBrush(taxi->getColor());
            p.drawPolygon(rombo);
            p.restore();

            if (taxi->esManual() && !esMinimapa)
            {
                dibujarTextoConBorde(p, d + QPointF(0, -54), QStringLiteral("DESTINO"),
                                     fuentePixel(14, true), taxi->getColor());
            }
        }
    }

    void dibujarTaxis(QPainter &p, bool esMinimapa)
    {
        for (int i = 0; i < sim->getCantidadTaxis(); i++)
        {
            Taxi *taxi = sim->getTaxi(i);
            QPointF pos = posicionDibujoTaxi(taxi);

            p.save();
            p.translate(pos);
            p.rotate(taxi->getAngulo());
            p.setPen(QPen(QColor(10, 14, 22), 2.2));
            p.setBrush(taxi->getColor());
            p.drawRoundedRect(QRectF(-14, -7.5, 28, 15), 5, 5);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(10, 14, 22, 215));
            p.drawRect(QRectF(2, -5.5, 6, 11));
            p.drawRect(QRectF(-11, -5, 4, 10));
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
            p.setPen(QPen(taxi->getColor(), 1.6));
            p.setBrush(QColor(8, 12, 20, 225));
            p.drawRoundedRect(caja, 6, 6);
            p.setFont(f);
            p.setPen(QColor(255, 255, 255));
            p.drawText(caja, Qt::AlignCenter, etiqueta);
        }
    }

    void dibujarEfectosNocturnos(QPainter &p)
    {
        if (oscuridad < 0.01)
        {
            return;
        }

        p.fillRect(sim->getLimitesMapa(), QColor(6, 12, 46, static_cast<int>(150 * oscuridad)));

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

    void dibujarHUD(QPainter &p)
    {
        const RelojSimulacion &reloj = sim->getReloj();
        QString periodo = reloj.periodoActual();

        QColor colorPeriodo("#4ade80");
        QString textoPeriodo = QStringLiteral("TRÁFICO NORMAL");
        if (periodo == "HoraPico")
        {
            colorPeriodo = QColor("#ff5252");
            textoPeriodo = QStringLiteral("HORA PICO ×1.8");
        }
        else if (periodo == "Nocturno")
        {
            colorPeriodo = QColor("#a78bfa");
            textoPeriodo = QStringLiteral("NOCTURNO +$ 2,000");
        }

        QRectF panelReloj(14, 14, 216, 124);
        p.setPen(QPen(QColor(255, 255, 255, 28), 1));
        p.setBrush(QColor(8, 12, 20, 218));
        p.drawRoundedRect(panelReloj, 14, 14);

        p.setFont(fuentePixel(42, true));
        p.setPen(QColor(255, 255, 255));
        p.drawText(QRectF(30, 18, 190, 52), Qt::AlignLeft | Qt::AlignVCenter, reloj.textoHora());

        p.setFont(fuentePixel(12, true));
        p.setPen(QColor("#93a4bd"));
        p.drawText(QRectF(32, 70, 190, 16), Qt::AlignLeft | Qt::AlignVCenter,
                   QStringLiteral("DÍA %1").arg(reloj.getDia()));

        QRectF insignia(28, 96, 188, 28);
        p.setPen(QPen(colorPeriodo, 1.8));
        p.setBrush(QColor(colorPeriodo.red(), colorPeriodo.green(), colorPeriodo.blue(), 50));
        p.drawRoundedRect(insignia, 9, 9);
        p.setFont(fuentePixel(13, true));
        p.setPen(colorPeriodo);
        p.drawText(insignia, Qt::AlignCenter, textoPeriodo);

        TaxiManual *yo = sim->getTaxiManual();
        if (yo == nullptr)
        {
            return;
        }

        QRectF panelDinero(width() - 334, 14, 320, 124);
        p.setPen(QPen(QColor(255, 255, 255, 28), 1));
        p.setBrush(QColor(8, 12, 20, 218));
        p.drawRoundedRect(panelDinero, 14, 14);

        p.setFont(fuentePixel(12, true));
        p.setPen(QColor("#93a4bd"));
        p.drawText(QRectF(panelDinero.left(), 22, panelDinero.width(), 16), Qt::AlignCenter,
                   QStringLiteral("TUS INGRESOS ACUMULADOS"));

        dibujarTextoConBorde(p, QPointF(panelDinero.center().x(), 76),
                             Grafo::formatearPesos(yo->getIngresos()), fuentePixel(40, true),
                             QColor("#7dff8a"), 5.0);

        p.setFont(fuentePixel(12));
        p.setPen(QColor("#dbe3ef"));
        p.drawText(QRectF(panelDinero.left() + 12, 86, panelDinero.width() - 24, 18),
                   Qt::AlignCenter, yo->textoEstado(sim->getGrafo()));
        p.setPen(QColor("#93a4bd"));
        p.drawText(QRectF(panelDinero.left() + 12, 104, panelDinero.width() - 24, 18),
                   Qt::AlignCenter,
                   QStringLiteral("Carreras completadas: %1").arg(yo->getViajes()));
    }

    void dibujarMinimapa(QPainter &p)
    {
        TaxiManual *yo = sim->getTaxiManual();
        if (yo == nullptr || width() < 700 || height() < 480)
        {
            return;
        }

        double radio = 96.0;
        QPointF centro(14 + radio, height() - 14 - radio);

        p.save();
        QPainterPath recorte;
        recorte.addEllipse(centro, radio, radio);
        p.setClipPath(recorte);
        p.fillRect(rect(), QColor("#0b1018"));

        QTransform t;
        t.translate(centro.x(), centro.y());
        t.scale(0.34, 0.34);
        t.translate(-yo->getPosicion().x(), -yo->getPosicion().y());
        p.setTransform(t);
        dibujarMundo(p, true);
        p.restore();

        p.setPen(QPen(QColor(255, 255, 255, 230), 3));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(centro, radio, radio);
    }

private slots:
    void enCuadro(double dt)
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

    void enCarreraCompletada(int, QPointF pos, double tarifa, QColor color)
    {
        textosFlotantes.append(
            TextoFlotante(pos, QStringLiteral("+") + Grafo::formatearPesos(tarifa), color));
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::TextAntialiasing);
        p.fillRect(rect(), QColor("#0b1018"));
        if (sim == nullptr)
        {
            return;
        }

        p.save();
        p.setTransform(transformacionVista());
        p.setClipRect(sim->getLimitesMapa());
        dibujarMundo(p, false);
        p.restore();

        dibujarHUD(p);
        dibujarMinimapa(p);
    }

    void mousePressEvent(QMouseEvent *e) override
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

    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (sim == nullptr)
        {
            return;
        }
        QPointF posMapa = transformacionVista().inverted().map(QPointF(e->pos()));
        nodoBajoMouse = sim->getGrafo()->nodoMasCercano(posMapa, 30.0);
        setCursor(nodoBajoMouse >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }

    void leaveEvent(QEvent *) override
    {
        nodoBajoMouse = -1;
    }

public:
    explicit MapaWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        sim = nullptr;
        oscuridad = 0.0;
        nodoBajoMouse = -1;
        nodoPendiente = -1;
        modoConducir = true;
        mostrarPesos = true;

        setMouseTracking(true);
        setMinimumSize(640, 460);
        relojAnimacion.start();
    }

    void setSimulacion(Simulacion *nuevaSim)
    {
        sim = nuevaSim;
        connect(sim, &Simulacion::cuadroActualizado, this, &MapaWidget::enCuadro);
        connect(sim, &Simulacion::carreraCompletada, this, &MapaWidget::enCarreraCompletada);
    }

    void setNodoPendiente(int idNodo) { nodoPendiente = idNodo; }
    void setModoConducir(bool activo) { modoConducir = activo; }
    void setMostrarPesos(bool mostrar) { mostrarPesos = mostrar; }

signals:
    void nodoClicado(int idNodo);
    void espacioVacioClicado(QPointF posMapa);
    void cancelarSolicitado();
};

#ifdef COMPILAR_MOCS
#include "MapaWidget.moc"
#endif
