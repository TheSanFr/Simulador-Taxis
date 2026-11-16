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
#include <QSet>
#include <QtMath>
#include <cmath>

namespace
{
const QColor fondoVista("#151a1d");
const QColor fondoPanel("#20262a");
const QColor bordePanel("#41494a");
const QColor textoPrincipal("#f0eee9");
const QColor textoSecundario("#b3bfbc");
const QColor colorManual("#ffd074");
const QColor colorSeleccion("#79e7d0");
const QColor colorTrafico("#ba6e68");
const QPointF desplazamientoCliente(-32, -24);
const QPointF desplazamientoDestino(31, -23);
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
    p.setPen(QPen(QColor(19, 23, 25, 235), grosorBorde, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    p.drawPath(rutaTexto);
    p.setPen(Qt::NoPen);
    p.setBrush(relleno);
    p.drawPath(rutaTexto);
}

QTransform MapaWidget::transformacionVista() const
{
    const QRectF limites = sim->getLimitesMapa();
    QRectF encuadre = limites;
    if (modoConducir)
    {
        encuadre.adjust(0.0, 52.0, 0.0, -48.0);
        const ElementoNodo *elemento = sim->getGrafo()->getNodos()->getPrimero();
        while (elemento != nullptr)
        {
            if (const Nodo *nodo = elemento->getNodo())
            {
                const QPointF pos = nodo->getPosicion();
                encuadre = encuadre.united(QRectF(pos - QPointF(65, 65), QSizeF(130, 130)));
            }
            elemento = elemento->getSiguiente();
        }
        encuadre = encuadre.intersected(limites);
    }
    constexpr double margen = 5.0;
    const double escala = qMin((width() - 2 * margen) / encuadre.width(),
                               (height() - 2 * margen) / encuadre.height());
    QTransform t;
    t.translate(width() / 2.0, height() / 2.0);
    t.scale(escala, escala);
    t.translate(-encuadre.center().x(), -encuadre.center().y());
    return t;
}

QPointF MapaWidget::posicionDibujoTaxi(const Taxi *taxi) const
{
    double rad = qDegreesToRadians(taxi->getAngulo());
    return taxi->getPosicion() + QPointF(-std::sin(rad), std::cos(rad)) * 8.0;
}

void MapaWidget::dibujarMundo(QPainter &p, bool esMinimapa)
{
    dibujarEscenario(p);
    dibujarCalles(p, esMinimapa);
    dibujarEfectosNocturnos(p);
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
    fondo.setColorAt(0.0, QColor("#303733"));
    fondo.setColorAt(1.0, QColor("#262e2c"));
    p.fillRect(b, fondo);

    const QPolygonF agua = sim->getPoligonoAgua();
    p.setPen(QPen(QColor("#48625f"), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QLinearGradient ribera(b.topLeft(), QPointF(b.left() + 235, b.top()));
    ribera.setColorAt(0.0, QColor("#193238"));
    ribera.setColorAt(1.0, QColor("#295055"));
    p.setBrush(ribera);
    p.drawPolygon(agua);

    const QVector<ManzanaCiudad> &manzanas = sim->getManzanas();
    for (int i = 0; i < manzanas.size(); i++)
    {
        const ManzanaCiudad &m = manzanas[i];
        const QString tipo = m.getTipo();
        const bool parque = tipo == "Parque";
        const bool industrial = tipo == "Industrial";
        const bool aeropuerto = tipo == "Aeropuerto";
        const bool terminal = tipo == "Terminal";
        const bool estadio = tipo == "Estadio";
        QColor color(i % 3 == 0 ? "#3a423d" : "#333d3a");
        if (parque || estadio)
            color = QColor("#334a3d");
        else if (industrial)
            color = QColor("#494439");
        else if (aeropuerto || terminal)
            color = QColor("#383e3c");

        const QPolygonF superficie = m.getPoligono();
        p.setPen(QPen(color.lighter(110), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(color);
        p.drawPolygon(superficie);

        const QRectF interior = superficie.boundingRect().adjusted(14, 16, -14, -16);
        if (interior.width() < 38.0 || interior.height() < 34.0)
            continue;

        QPainterPath recorte;
        recorte.addPolygon(superficie);
        recorte.closeSubpath();
        p.save();
        p.setClipPath(recorte, Qt::IntersectClip);
        p.setPen(Qt::NoPen);
        const QPointF centro = interior.center();
        const auto techo = [&p](QRectF volumen, const QColor &colorTecho) {
            p.setBrush(QColor(21, 27, 27, 142));
            p.drawRoundedRect(volumen.translated(4, 5), 3, 3);
            p.setBrush(colorTecho);
            p.drawRoundedRect(volumen, 3, 3);
        };

        if (aeropuerto)
        {
            p.save();
            p.translate(centro);
            p.rotate(-18);
            const double largo = interior.width() * 0.78;
            const QRectF pista(-largo / 2, -17, largo, 34);
            p.setBrush(QColor("#272d2e"));
            p.drawRoundedRect(pista, 4, 4);
            QPen eje(QColor("#929792"), 1.6, Qt::DashLine, Qt::FlatCap);
            p.setPen(eje);
            p.drawLine(QPointF(pista.left() + 13, 0), QPointF(pista.right() - 13, 0));
            p.setPen(QPen(QColor("#7f8781"), 2.5));
            p.drawLine(QPointF(pista.left() + 10, -10), QPointF(pista.left() + 10, 10));
            p.drawLine(QPointF(pista.right() - 10, -10), QPointF(pista.right() - 10, 10));
            p.restore();
        }
        else if (estadio)
        {
            const QRectF gradas(centro.x() - 54, centro.y() - 35, 108, 70);
            p.setBrush(QColor("#62665a"));
            p.drawRoundedRect(gradas, 35, 35);
            p.setBrush(QColor("#263933"));
            p.drawRoundedRect(gradas.adjusted(9, 8, -9, -8), 26, 26);
            const QRectF cancha(centro.x() - 33, centro.y() - 18, 66, 36);
            p.setPen(QPen(QColor("#718575"), 1.4));
            p.setBrush(QColor("#48614f"));
            p.drawRect(cancha);
            p.drawLine(QPointF(centro.x(), cancha.top()), QPointF(centro.x(), cancha.bottom()));
            p.drawEllipse(centro, 6, 6);
        }
        else if (parque)
        {
            QPainterPath sendero(interior.bottomLeft() + QPointF(8, -10));
            sendero.cubicTo(centro + QPointF(-46, 12), centro + QPointF(20, -6),
                            interior.topRight() + QPointF(-7, 10));
            p.setPen(QPen(QColor("#687661"), 5, Qt::SolidLine, Qt::RoundCap));
            p.setBrush(Qt::NoBrush);
            p.drawPath(sendero);
            p.setPen(Qt::NoPen);
            const QPointF arboles[] = {QPointF(-48, -26), QPointF(-32, -36), QPointF(-23, -15),
                                       QPointF(41, 25), QPointF(22, 36), QPointF(53, 35)};
            for (const QPointF &arbol : arboles)
            {
                p.setBrush(QColor("#263d33"));
                p.drawEllipse(centro + arbol + QPointF(3, 4), 10, 10);
                p.setBrush(QColor("#526e59"));
                p.drawEllipse(centro + arbol, 9, 9);
            }
        }
        else
        {
            const double ancho = interior.width();
            const double alto = interior.height();
            if (industrial || terminal)
            {
                techo(QRectF(centro.x() - ancho * 0.30, centro.y() - alto * 0.31,
                             ancho * 0.58, alto * 0.25), color.lighter(125));
                techo(QRectF(centro.x() - ancho * 0.16, centro.y() + alto * 0.09,
                             ancho * 0.43, alto * 0.19), color.lighter(116));
            }
            else
            {
                const double signo = i % 2 == 0 ? 1.0 : -1.0;
                const QPointF principal = centro + QPointF(signo * ancho * 0.15, -alto * 0.13);
                techo(QRectF(principal.x() - ancho * 0.16, principal.y() - alto * 0.12,
                             ancho * 0.32, alto * 0.24), color.lighter(116));
                techo(QRectF(centro.x() - signo * ancho * 0.23 - 13, centro.y() + alto * 0.09,
                             26, alto * 0.25), color.lighter(112));
                if (i % 3 == 0)
                    techo(QRectF(centro.x() + signo * ancho * 0.11 - 13, centro.y() + alto * 0.17,
                                 26, alto * 0.17), color.lighter(112));
            }
        }
        p.restore();
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
        p.setPen(QPen(QColor("#565d56"), av ? 42 : 30, Qt::SolidLine, Qt::RoundCap));
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
        QColor base = av ? QColor("#414746") : QColor("#373e3d");
        QColor c = mezclarColores(base, colorTrafico,
                                  qBound(0.0, (g->obtenerNivelCongestion(arista) - 1.05) * 0.14, 0.26));
        p.setPen(QPen(c, av ? 35 : 24, Qt::SolidLine, Qt::RoundCap));
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
        if (tieneInversa && arista->getEsAvenida() && linea.length() > 70.0)
        {
            QPen marcas(QColor(216, 212, 186, 95), 1.4, Qt::CustomDashLine, Qt::FlatCap);
            marcas.setDashPattern({6, 7});
            p.setPen(marcas);
            p.drawLine(a + sentido * 22, b - sentido * 22);
        }
        p.setPen(QPen(tieneInversa ? QColor("#a5aaa1") : QColor("#e8dfc9"),
                      tieneInversa ? (modoConducir && taxiEnFoco >= 0 ? 1.3 : 2.0) : 2.8,
                      Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto chevron = [&p](QPointF centro, QPointF direccion) {
            const QPointF lado(-direccion.y(), direccion.x());
            const QPointF puntos[] = {
                centro - direccion * 4.5 + lado * 3.2,
                centro + direccion * 4.5,
                centro - direccion * 4.5 - lado * 3.2
            };
            p.drawPolyline(puntos, 3);
        };
        const QPointF haciaDerecha(-sentido.y(), sentido.x());
        const double carril = arista->getEsAvenida() ? 9.0 : 7.0;
        chevron(linea.pointAt(tieneInversa ? 0.63 : 0.50)
                    + (tieneInversa ? haciaDerecha * carril : QPointF()), sentido);
        if (tieneInversa)
        {
            chevron(linea.pointAt(0.37) - haciaDerecha * carril, -sentido);
        }
    }
}

void MapaWidget::dibujarRutas(QPainter &p)
{
    Grafo *g = sim->getGrafo();
    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        if (!mostrarRutaDelTaxi(taxi))
        {
            continue;
        }
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
        const bool detalle = taxi->esManual() || taxiEnFoco >= 0;
        c.setAlpha(detalle ? 255 : 160);
        p.setBrush(Qt::NoBrush);
        if (detalle)
        {
            p.setPen(QPen(fondoVista, 10.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawPolyline(poli);
        }
        p.setPen(QPen(c, detalle ? 5.5 : 3.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(poli);
    }

    // La vista previa solo representa la calle que admitiría el siguiente clic.
    const TaxiManual *manual = sim->getTaxiManual();
    if (!modoConducir || manual == nullptr || nodoBajoMouse < 0 || !mostrarRutaDelTaxi(manual))
        return;

    const int salida = manual->nodoFinalDeRuta();
    const Nodo *origen = g->buscarNodo(salida);
    const Nodo *entrada = g->buscarNodo(nodoBajoMouse);
    if (origen == nullptr || entrada == nullptr || !g->existeAristaDirigida(salida, nodoBajoMouse))
        return;

    const QLineF tramo(origen->getPosicion(), entrada->getPosicion());
    if (tramo.length() < 1.0)
        return;
    QColor colorVistaPrevia = colorManual;
    colorVistaPrevia.setAlpha(200);
    QPen vistaPrevia(colorVistaPrevia, 4.0, Qt::CustomDashLine, Qt::RoundCap);
    vistaPrevia.setDashPattern({1.2, 2.1});
    p.setPen(vistaPrevia);
    p.drawLine(tramo.pointAt(0.06), tramo.pointAt(0.87));

    const QPointF direccion = (tramo.p2() - tramo.p1()) / tramo.length();
    const QPointF normal(-direccion.y(), direccion.x());
    const QPointF punta = tramo.pointAt(0.88);
    p.setPen(QPen(colorManual, 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(QPolygonF{punta - direccion * 11 + normal * 6, punta,
                             punta - direccion * 11 - normal * 6});
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
    const TaxiManual *manual = sim->getTaxiManual();
    QSet<int> salidasManuales;
    if (modoConducir && manual != nullptr && !esMinimapa)
    {
        const int desde = manual->nodoFinalDeRuta();
        for (const ElementoArista *e = g->getAristas()->getPrimero(); e != nullptr; e = e->getSiguiente())
        {
            const Arista *arista = e->getArista();
            if (arista != nullptr && arista->getOrigen() == desde)
                salidasManuales.insert(arista->getDestino());
        }
    }
    const bool conduccionEnFoco = manual != nullptr && mostrarRutaDelTaxi(manual);
    int destinoVisible = -1;
    for (int i = 0; i < sim->getCantidadTaxis(); ++i)
    {
        const Taxi *taxi = sim->getTaxi(i);
        if (taxi->getLlevaPasajero()
            && (taxi->getId() == taxiEnFoco || (taxiEnFoco < 0 && taxi->esManual())))
        {
            destinoVisible = taxi->getDestinoPasajeroABordo();
            break;
        }
    }

    ElementoNodo *elem = g->getNodos()->getPrimero();
    while (elem != nullptr)
    {
        Nodo *n = elem->getNodo();
        elem = elem->getSiguiente();
        if (n == nullptr)
        {
            continue;
        }

        const bool especial = n->getTipo() != "Normal";
        const bool resaltado = n->getId() == nodoBajoMouse;
        const bool disponible = salidasManuales.contains(n->getId());
        const QColor colorTipo = obtenerColorTipoNodo(n->getTipo());
        const double radio = especial ? (esMinimapa ? 12.0 : 14.0) : 6.5;

        if (disponible)
        {
            QColor borde = colorManual;
            borde.setAlpha(resaltado ? 255 : conduccionEnFoco ? 155 : 98);
            p.setPen(QPen(borde, resaltado ? 3.5 : 2.0));
            p.setBrush(QColor(23, 27, 28, 150));
            p.drawEllipse(n->getPosicion(), 20.0, 20.0);
        }
        else if (!esMinimapa && resaltado)
        {
            const double avance = animacionNodo.currentValue().toDouble();
            QColor borde = textoSecundario;
            borde.setAlphaF(0.60 * avance);
            p.setPen(QPen(borde, 2.0));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(n->getPosicion(), radio + 7.0 * avance, radio + 7.0 * avance);
        }

        p.setPen(QPen(fondoVista, especial ? 3.2 : 2.6));
        p.setBrush(especial ? colorTipo : disponible ? colorManual : QColor("#9ca9a4"));
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

        const bool nombreVisible = especial || resaltado || n->getId() == destinoVisible
            || !modoConducir;
        if (!esMinimapa && nombreVisible)
        {
            const double escala = p.worldTransform().m11();
            const int px = qBound(especial ? 16 : 14, qRound(9.0 / escala), 24);
            const QFont f = fuentePixel(px, especial || resaltado);
            const QColor c = resaltado || n->getId() == destinoVisible ? textoPrincipal
                : especial ? mezclarColores(colorTipo, textoSecundario, 0.42) : textoSecundario;
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

}

void MapaWidget::dibujarPasajeros(QPainter &p, bool esMinimapa)
{
    Grafo *g = sim->getGrafo();
    const QVector<Pasajero> &lista = sim->getPasajerosEnEspera();
    for (int i = 0; i < lista.size(); i++)
    {
        const Pasajero &pas = lista[i];
        Nodo *no = g->buscarNodo(pas.getOrigen());
        if (no == nullptr)
        {
            continue;
        }

        const QPointF o = no->getPosicion();
        const QPointF ubicacion = o + desplazamientoCliente;
        const bool resaltado = !esMinimapa && no->getId() == nodoBajoMouse
            && QLineF(posicionCursorMapa, ubicacion).length() <= 16.0;

        p.save();
        p.setPen(QPen(colorSeleccion, resaltado ? 2.5 : 1.6, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(ubicacion, o);
        p.translate(ubicacion);
        p.setPen(QPen(fondoVista, 2.4));
        p.setBrush(colorSeleccion);
        p.drawEllipse(QPointF(), 15.0, 15.0);
        p.setPen(Qt::NoPen);
        p.setBrush(fondoVista);
        p.drawEllipse(QPointF(0, -4), 3.6, 3.6);
        p.drawRoundedRect(QRectF(-6, 1, 12, 6), 2.5, 2.5);
        p.restore();

        if (resaltado)
        {
            const QFont f = fuentePixel(qBound(16, qRound(11.0 / p.worldTransform().m11()), 27), true);
            const QFontMetricsF fm(f);
            const QString texto = fm.elidedText(
                QStringLiteral("Cliente · %1").arg(no->getNombre()), Qt::ElideRight, 350);
            const double ancho = fm.horizontalAdvance(texto) + 22;
            const double alto = fm.height() + 10;
            const QRectF limites = sim->getLimitesMapa().intersected(p.clipBoundingRect());
            const double x = qBound(limites.left() + 8, ubicacion.x() - ancho / 2, limites.right() - ancho - 8);
            const double arriba = ubicacion.y() - alto - 19;
            const double y = arriba >= limites.top() + 6 ? arriba : ubicacion.y() + 28;
            const QRectF globo(x, y, ancho, alto);
            p.setPen(QPen(colorSeleccion, 1.8));
            p.setBrush(fondoPanel);
            p.drawRoundedRect(globo, 7, 7);
            p.setFont(f);
            p.setPen(textoPrincipal);
            p.drawText(globo, Qt::AlignCenter, texto);
        }
    }

    QSet<int> destinosDibujados;
    for (int i = sim->getCantidadTaxis() - 1; i >= 0; --i)
    {
        Taxi *taxi = sim->getTaxi(i);
        if (!taxi->getLlevaPasajero() || !mostrarRutaDelTaxi(taxi)
            || destinosDibujados.contains(taxi->getDestinoPasajeroABordo()))
        {
            continue;
        }
        Nodo *nd = g->buscarNodo(taxi->getDestinoPasajeroABordo());
        if (nd == nullptr)
        {
            continue;
        }
        destinosDibujados.insert(nd->getId());
        QPointF d = nd->getPosicion();
        const QPointF ubicacion = d + desplazamientoDestino;
        p.save();
        const QColor colorDestino = taxi->esManual() ? colorManual : taxi->getColor();
        p.setPen(QPen(colorDestino, 1.8, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(d, ubicacion);
        p.translate(ubicacion);
        p.setPen(QPen(colorDestino, 2.3));
        p.setBrush(fondoPanel);
        p.drawRoundedRect(QRectF(-16, -16, 32, 32), 7, 7);
        p.setPen(QPen(colorDestino, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(QPointF(-6, 9), QPointF(-6, -9));
        p.setBrush(colorDestino);
        p.drawPolygon(QPolygonF{QPointF(-6, -9), QPointF(8, -6), QPointF(-6, -2)});
        p.restore();

        if (!esMinimapa && (taxi->esManual() || taxiEnFoco >= 0))
        {
            const int px = qBound(17, qRound(10.0 / p.worldTransform().m11()), 26);
            dibujarTextoConBorde(p, d + QPointF(31, -55), QStringLiteral("Destino"),
                                 fuentePixel(px, true), colorDestino);
        }
    }
}

void MapaWidget::dibujarTaxis(QPainter &p, bool esMinimapa)
{
    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        const bool destacado = mostrarRutaDelTaxi(taxi);
        QPointF pos = posicionDibujoTaxi(taxi);

        p.save();
        p.translate(pos);
        p.rotate(taxi->getAngulo());
        const QColor color = taxi->esManual() ? colorManual
            : destacado ? taxi->getColor() : mezclarColores(taxi->getColor(), QColor("#a7b0ae"), 0.43);
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

        if (esMinimapa || (!destacado && !taxi->esManual()))
        {
            continue;
        }

        QString etiqueta = taxi->getNombre();
        if (taxi->getLlevaPasajero() && (taxi->esManual() || taxiEnFoco >= 0))
        {
            etiqueta += QStringLiteral(" · A bordo");
        }
        else if (taxi->getLlevaPasajero())
        {
            etiqueta += QStringLiteral(" ●");
        }
        const QFont f = fuentePixel(qBound(15, qRound(9.0 / p.worldTransform().m11()), 23), true);
        const QFontMetricsF fm(f);
        const double ancho = fm.horizontalAdvance(etiqueta) + 16;
        const double alto = fm.height() + 5;
        const QRectF caja(pos.x() - ancho / 2, pos.y() - alto - 18, ancho, alto);
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

    p.fillRect(sim->getLimitesMapa(), QColor(16, 22, 31, static_cast<int>(110 * oscuridad)));

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        p.save();
        p.translate(posicionDibujoTaxi(taxi));
        p.rotate(taxi->getAngulo());
        QLinearGradient haz(10, 0, 70, 0);
        const int intensidad = taxi->esManual() ? 170 : mostrarRutaDelTaxi(taxi) ? 85 : 43;
        haz.setColorAt(0.0, QColor(255, 244, 200, static_cast<int>(intensidad * oscuridad)));
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
    if (taxiEnFoco >= 0 || width() < 1000 || height() < 575)
    {
        return;
    }
    const Taxi *referencia = sim->getTaxiManual();
    if (referencia == nullptr)
        return;

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
    t.translate(-referencia->getPosicion().x(), -referencia->getPosicion().y());
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

void MapaWidget::enCarreraCompletada(int idTaxi, QPointF pos, double tarifa, QColor color)
{
    if (taxiEnFoco >= 0 && taxiEnFoco != idTaxi)
        return;
    textosFlotantes.append(
        TextoFlotante(pos, QStringLiteral("Llegada · +") + Grafo::formatearPesos(tarifa), color));
}

void MapaWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.fillRect(rect(), fondoVista);
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

    QPointF posMapa = transformacionVista().inverted().map(e->position());
    const int idNodo = nodoInteractivo(posMapa);
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
    posicionCursorMapa = transformacionVista().inverted().map(e->position());
    seleccionarNodoEnFoco(nodoInteractivo(posicionCursorMapa));
}

int MapaWidget::nodoInteractivo(QPointF posMapa) const
{
    const Grafo *grafo = sim->getGrafo();
    if (modoConducir)
    {
        for (const Pasajero &pasajero : sim->getPasajerosEnEspera())
        {
            const Nodo *origen = grafo->buscarNodo(pasajero.getOrigen());
            if (origen != nullptr && QLineF(posMapa, origen->getPosicion() + desplazamientoCliente).length() <= 16.0)
                return origen->getId();
        }
        for (int i = 0; i < sim->getCantidadTaxis(); ++i)
        {
            const Taxi *taxi = sim->getTaxi(i);
            if (!taxi->getLlevaPasajero() || !mostrarRutaDelTaxi(taxi))
                continue;
            const Nodo *destino = grafo->buscarNodo(taxi->getDestinoPasajeroABordo());
            if (destino != nullptr
                && QRectF(destino->getPosicion() + desplazamientoDestino - QPointF(16, 16), QSizeF(32, 32)).contains(posMapa))
                return destino->getId();
        }
    }
    return grafo->nodoMasCercano(posMapa, 30.0);
}

void MapaWidget::leaveEvent(QEvent *)
{
    seleccionarNodoEnFoco(-1);
}

void MapaWidget::seleccionarNodoEnFoco(int id)
{
    const TaxiManual *manual = modoConducir && sim != nullptr ? sim->getTaxiManual() : nullptr;
    const bool permiteClic = id >= 0 && (!modoConducir ||
        (manual != nullptr && sim->getGrafo()->existeAristaDirigida(manual->nodoFinalDeRuta(), id)));
    setCursor(permiteClic ? Qt::PointingHandCursor : Qt::ArrowCursor);
    if (nodoBajoMouse == id)
    {
        return;
    }

    nodoBajoMouse = id;
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

bool MapaWidget::mostrarRutaDelTaxi(const Taxi *taxi) const
{
    return taxiEnFoco < 0 || taxi->getId() == taxiEnFoco;
}

void MapaWidget::setTaxiEnFoco(int idTaxi)
{
    taxiEnFoco = idTaxi;
    textosFlotantes.clear();
    update();
}
