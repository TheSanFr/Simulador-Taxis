#include "interfaz/MapaWidget.h"

#include "modelo/Arista.h"
#include "estructuras/ElementoArista.h"
#include "estructuras/ElementoNodo.h"
#include "simulacion/Grafo.h"
#include "estructuras/ListaAristas.h"
#include "estructuras/ListaNodos.h"
#include "interfaz/MainWindow.h"
#include "modelo/ManzanaCiudad.h"
#include "modelo/Nodo.h"
#include "modelo/Pasajero.h"
#include "simulacion/RelojSimulacion.h"
#include "simulacion/Simulacion.h"
#include "taxis/Taxi.h"
#include "taxis/TaxiManual.h"

#include <QEasingCurve>
#include <QEvent>
#include <QFontMetricsF>
#include <QLineF>
#include <QLinearGradient>
#include <QLocale>
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

const QColor fondoVista("#151a1d");
const QColor fondoPanel("#20262a");
const QColor bordePanel("#41494a");
const QColor textoPrincipal("#f0eee9");
const QColor textoSecundario("#b3bfbc");
const QColor colorManual("#ffd074");
const QColor colorSeleccion("#79e7d0");
const QColor colorTrafico("#ba6e68");
const QColor colorEdicionInvalida("#fa9b8b");
const QPointF desplazamientoCliente(-32, -24);
const QPointF desplazamientoDestino(31, -23);

double anchoConAcera(bool avenida)
{
    return avenida ? 42.0 : 30.0;
}

QPainterPath corredorDeTramo(QPointF origen, QPointF destino, double ancho)
{
    QPainterPath eje(origen);
    eje.lineTo(destino);
    QPainterPathStroker trazo;
    trazo.setWidth(ancho);
    trazo.setCapStyle(Qt::RoundCap);
    trazo.setJoinStyle(Qt::RoundJoin);
    return trazo.createStroke(eje);
}

void calcularFlecha(QPointF centro, QPointF direccion, QPointF puntos[])
{
    const QPointF lado(-direccion.y(), direccion.x());
    puntos[0] = centro - direccion * 4.5 + lado * 3.2;
    puntos[1] = centro + direccion * 4.5;
    puntos[2] = centro - direccion * 4.5 - lado * 3.2;
}

void dibujarFlechaPrevia(QPainter &p, QPointF punta, QPointF avance)
{
    const QPointF lateral(-avance.y(), avance.x());
    QPolygonF poligono;
    poligono << (punta - avance * 12 + lateral * 7)
             << punta
             << (punta - avance * 12 - lateral * 7);
    p.drawPolyline(poligono);
}

TramoVisible::TramoVisible()
    : visible(false), avenida(false), dobleSentido(false), dibujarFlechas(false)
{
}

QLineF TramoVisible::getEje() const
{
    return eje;
}

void TramoVisible::setEje(QLineF nuevoEje)
{
    eje = nuevoEje;
}

QLineF TramoVisible::getLineaCentral() const
{
    return lineaCentral;
}

void TramoVisible::setLineaCentral(QLineF nuevaLinea)
{
    lineaCentral = nuevaLinea;
}

const QPointF *TramoVisible::getFlechaIda() const
{
    return flechaIda;
}

QPointF *TramoVisible::datosFlechaIda()
{
    return flechaIda;
}

const QPointF *TramoVisible::getFlechaVuelta() const
{
    return flechaVuelta;
}

QPointF *TramoVisible::datosFlechaVuelta()
{
    return flechaVuelta;
}

bool TramoVisible::esVisible() const
{
    return visible;
}

void TramoVisible::setVisible(bool valor)
{
    visible = valor;
}

bool TramoVisible::esAvenida() const
{
    return avenida;
}

void TramoVisible::setAvenida(bool valor)
{
    avenida = valor;
}

bool TramoVisible::esDobleSentido() const
{
    return dobleSentido;
}

void TramoVisible::setDobleSentido(bool valor)
{
    dobleSentido = valor;
}

bool TramoVisible::debeDibujarFlechas() const
{
    return dibujarFlechas;
}

void TramoVisible::setDibujarFlechas(bool valor)
{
    dibujarFlechas = valor;
}

NombreNodo::NombreNodo()
    : id(-1), especial(false)
{
}

NombreNodo::NombreNodo(int id, QPointF posicion, QStaticText texto, QColor color, bool especial)
    : id(id), posicion(posicion), texto(texto), color(color), especial(especial)
{
}

int NombreNodo::getId() const
{
    return id;
}

QPointF NombreNodo::getPosicion() const
{
    return posicion;
}

const QStaticText &NombreNodo::getTexto() const
{
    return texto;
}

QColor NombreNodo::getColor() const
{
    return color;
}

bool NombreNodo::esEspecial() const
{
    return especial;
}

QFont MapaWidget::fuentePixel(int px, bool negrita) const
{
    QFont f;
    f.setFamilies({QStringLiteral("Bahnschrift"), QStringLiteral("Segoe UI")});
    f.setPixelSize(px);
    f.setBold(negrita);
    return f;
}

QColor MapaWidget::obtenerColorTipoNodo(QString tipo) const
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

QColor MapaWidget::mezclarColores(QColor a, QColor b, double t) const
{
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;
    return QColor::fromRgbF(a.redF() + (b.redF() - a.redF()) * t,
                            a.greenF() + (b.greenF() - a.greenF()) * t,
                            a.blueF() + (b.blueF() - a.blueF()) * t);
}

void MapaWidget::dibujarTextoConBorde(QPainter &p, QPointF centro, QString texto,
                                      QFont fuente, QColor relleno, double grosorBorde) const
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

QRectF MapaWidget::areaMarcoMapa() const
{
    return QRectF(rect()).adjusted(6.0, 6.0, -6.0, -6.0);
}

void MapaWidget::sincronizarRevisionGrafo()
{
    if (sim == nullptr || sim->getGrafo() == nullptr)
        return;
    const int actual = sim->getGrafo()->getRevisionGrafo();
    if (actual != ultimaRevisionGrafo)
    {
        ultimaRevisionGrafo = actual;
        invalidarGeometria();
        if (!sim->getGrafo()->tieneNodo(nodoBajoMouse))
        {
            seleccionarNodoEnFoco(-1);
        }
    }
}

QTransform MapaWidget::transformacionVista()
{
    sincronizarRevisionGrafo();
    actualizarGeometria();
    const QRectF encuadre = modoConducir ? encuadreConduccion : sim->getLimitesMapa();
    const QRectF marco = areaMarcoMapa();
    QRectF disponible = marco;
    double escala = qMin(disponible.width() / encuadre.width(),
                         disponible.height() / encuadre.height());

    if (modoConducir)
    {
        const double xMirador = disponible.center().x() + (520.0 - encuadre.center().x()) * escala;
        const double ySuperior = disponible.center().y() + (110.0 - encuadre.center().y()) * escala;
        if (xMirador < 215.0 && ySuperior < 96.0)
        {
            const double extraSuperior = qMin(44.0, 96.0 - ySuperior);
            disponible.setTop(disponible.top() + extraSuperior);
            escala = qMin(disponible.width() / encuadre.width(),
                          disponible.height() / encuadre.height());
        }
    }

    QTransform t;
    t.translate(disponible.center().x(), disponible.center().y());
    t.scale(escala, escala);
    t.translate(-encuadre.center().x(), -encuadre.center().y());
    return t;
}

void MapaWidget::actualizarGeometria()
{
    if (!geometriaPendiente)
        return;

    const Grafo *grafo = sim->getGrafo();
    const QRectF limites = sim->getLimitesMapa();
    QRectF cajaNodos = limites;
    for (const ElementoNodo *elemento = grafo->getNodos()->getPrimero(); elemento != nullptr;
         elemento = elemento->getSiguiente())
    {
        if (const Nodo *nodo = elemento->getNodo())
        {
            const QPointF posicion = nodo->getPosicion();
            const double margenArriba = (posicion.x() < 330.0 && posicion.y() < 160.0) ? 112.0 : 68.0;
            cajaNodos = cajaNodos.united(
                QRectF(posicion.x() - 75.0, posicion.y() - margenArriba, 150.0, margenArriba + 95.0));
        }
    }
    encuadreConduccion = cajaNodos.intersected(limites);

    const ListaAristas *aristas = grafo->getAristas();
    QSet<QPair<int, int>> sentidos;
    sentidos.reserve(aristas->getCantidad());
    for (const ElementoArista *elemento = aristas->getPrimero(); elemento != nullptr;
         elemento = elemento->getSiguiente())
    {
        if (const Arista *arista = elemento->getArista())
            sentidos.insert(QPair<int, int>(arista->getOrigen(), arista->getDestino()));
    }

    tramosPorArista.clear();
    tramosPorArista.reserve(aristas->getCantidad());
    for (const ElementoArista *elemento = aristas->getPrimero(); elemento != nullptr;
         elemento = elemento->getSiguiente())
    {
        tramosPorArista.append(TramoVisible());
        TramoVisible &tramo = tramosPorArista.last();
        const Arista *arista = elemento->getArista();
        if (arista == nullptr)
            continue;

        const int origen = arista->getOrigen();
        const int destino = arista->getDestino();
        const bool doble = sentidos.contains(QPair<int, int>(destino, origen));
        tramo.setDobleSentido(doble);
        if (doble && origen > destino)
            continue;

        const Nodo *desde = grafo->buscarNodo(origen);
        const Nodo *hasta = grafo->buscarNodo(destino);
        if (desde == nullptr || hasta == nullptr)
            continue;

        tramo.setVisible(true);
        tramo.setAvenida(arista->getEsAvenida());
        const QLineF eje(desde->getPosicion(), hasta->getPosicion());
        tramo.setEje(eje);
        const double longitud = eje.length();
        if (longitud < 34.0)
            continue;

        tramo.setDibujarFlechas(true);
        const QPointF sentido = (eje.p2() - eje.p1()) / longitud;
        if (doble && tramo.esAvenida() && longitud > 70.0)
            tramo.setLineaCentral(QLineF(eje.p1() + sentido * 22, eje.p2() - sentido * 22));

        const QPointF derecha(-sentido.y(), sentido.x());
        const double carril = tramo.esAvenida() ? 9.0 : 7.0;
        calcularFlecha(eje.pointAt(doble ? 0.63 : 0.50)
            + (doble ? derecha * carril : QPointF()), sentido, tramo.datosFlechaIda());
        if (doble)
            calcularFlecha(eje.pointAt(0.37) - derecha * carril, -sentido, tramo.datosFlechaVuelta());
    }
    geometriaPendiente = false;
}

void MapaWidget::invalidarGeometria()
{
    geometriaPendiente = true;
    corredorPendiente = true;
    escalaNombres = 0.0;
    temporizadorRecorrido.stop();
}

QPointF MapaWidget::posicionDibujoTaxi(const Taxi *taxi) const
{
    double rad = qDegreesToRadians(taxi->getAngulo());
    return taxi->getPosicion() + QPointF(-std::sin(rad), std::cos(rad)) * 8.0;
}

void MapaWidget::dibujarTechoManzana(QPainter &p, QRectF volumen, QColor colorTecho) const
{
    if (corredorCalles.intersects(volumen.adjusted(-1, -1, 4, 5)))
        return;
    p.setBrush(QColor(21, 27, 27, 142));
    p.drawRoundedRect(volumen.translated(4, 5), 3, 3);
    p.setBrush(colorTecho);
    p.drawRoundedRect(volumen, 3, 3);
}

bool MapaWidget::ubicarHitoManzana(const QPainterPath &recorte, QPointF centro,
                                   const QPainterPath &huella, QPointF &posicionElegida) const
{
    const QPointF ajustes[5] = {QPointF(), QPointF(-29, 0), QPointF(29, 0),
                                QPointF(0, -22), QPointF(0, 22)};
    for (int i = 0; i < 5; ++i)
    {
        const QPointF posicion = centro + ajustes[i];
        const QPainterPath ocupada = huella.translated(posicion);
        if (recorte.contains(ocupada) && !corredorCalles.intersects(ocupada))
        {
            posicionElegida = posicion;
            return true;
        }
    }
    return false;
}

void MapaWidget::dibujarRotuloEdicion(QPainter &p, QPointF referencia, QString texto, QColor acento) const
{
    if (texto.isEmpty())
        return;
    const QRectF limites = sim->getLimitesMapa();
    const double escala = p.worldTransform().m11();
    const QFont f = fuentePixel(qBound(14, qRound(11.0 / escala), 25), true);
    const QFontMetricsF medidas(f);
    const double ancho = qMin(medidas.horizontalAdvance(texto) + 24.0, limites.width() - 24.0);
    const double alto = medidas.height() + 10.0;
    QPointF centro(qBound(limites.left() + ancho / 2 + 10.0, referencia.x(),
                          limites.right() - ancho / 2 - 10.0),
                   qBound(limites.top() + alto / 2 + 8.0, referencia.y(),
                          limites.bottom() - alto / 2 - 8.0));
    const QRectF caja(centro.x() - ancho / 2, centro.y() - alto / 2, ancho, alto);
    p.setFont(f);
    p.setPen(QPen(acento, 1.3));
    p.setBrush(fondoPanel);
    p.drawRoundedRect(caja, 8, 8);
    p.setPen(textoPrincipal);
    p.drawText(caja, Qt::AlignCenter, medidas.elidedText(texto, Qt::ElideRight, qFloor(ancho - 14.0)));
}

void MapaWidget::dibujarMarcaRecorrido(QPainter &p, QRectF limites, QFontMetricsF medidas,
                                       QPointF cruce, QString texto, QColor color, bool recogida) const
{
    const double ancho = medidas.horizontalAdvance(texto) + 20;
    const double alto = medidas.height() + 9;
    const double x = qBound(limites.left(), cruce.x() - ancho / 2,
                            qMax(limites.left(), limites.right() - ancho));
    const double y = qBound(limites.top(), recogida ? cruce.y() - 26 - alto : cruce.y() + 24,
                            qMax(limites.top(), limites.bottom() - alto));
    const QRectF rotulo(x, y, ancho, alto);
    const QPointF ancla(rotulo.center().x(), recogida ? rotulo.bottom() : rotulo.top());
    p.setPen(QPen(color, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    if (recogida)
        p.drawEllipse(cruce, 14, 14);
    else
    {
        QPolygonF rombo;
        rombo << (cruce + QPointF(0, -15)) << (cruce + QPointF(15, 0))
              << (cruce + QPointF(0, 15)) << (cruce + QPointF(-15, 0));
        p.drawPolygon(rombo);
    }
    const QLineF guia(cruce, ancla);
    if (guia.length() > 16)
        p.drawLine(guia.pointAt(15 / guia.length()), ancla);

    p.setBrush(fondoPanel);
    p.drawRoundedRect(rotulo, 6, 6);
    p.setPen(color);
    p.drawText(rotulo, Qt::AlignCenter, texto);
}

void MapaWidget::dibujarMundo(QPainter &p, bool esMinimapa)
{
    dibujarFondoCiudad(p, esMinimapa);
    dibujarCalles(p, esMinimapa);
    dibujarEfectosNocturnos(p);
    if (!esMinimapa)
    {
        dibujarVistaPreviaEdicion(p);
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
        dibujarConfirmacion(p);
        dibujarRecorridoConsultado(p);
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

void MapaWidget::dibujarFondoCiudad(QPainter &p, bool esMinimapa)
{
    const QRectF mundo = sim->getLimitesMapa().adjusted(-300.0, -120.0, 300.0, 120.0);
    if (!esMinimapa)
    {
        const double maximoPixeles = 2400000.0;
        const double escala = qMin(p.worldTransform().m11() * p.device()->devicePixelRatioF(),
                                   std::sqrt(maximoPixeles / (mundo.width() * mundo.height())));
        const QSize pixeles(qMax(1, qCeil(mundo.width() * escala)), qMax(1, qCeil(mundo.height() * escala)));
        if (corredorPendiente || fondoCiudad.size() != pixeles)
        {
            fondoCiudad = QPixmap();
            QPixmap nuevo(pixeles);
            if (!nuevo.isNull())
            {
                nuevo.fill(Qt::transparent);
                {
                    QPainter terreno(&nuevo);
                    terreno.setRenderHint(QPainter::Antialiasing);
                    terreno.scale(pixeles.width() / mundo.width(), pixeles.height() / mundo.height());
                    terreno.translate(-mundo.left(), -mundo.top());
                    dibujarEscenario(terreno);
                }
                fondoCiudad = nuevo;
            }
        }
    }
    if (fondoCiudad.isNull())
        dibujarEscenario(p);
    else
        p.drawPixmap(mundo, fondoCiudad, QRectF(fondoCiudad.rect()));
}

void MapaWidget::actualizarCorredorCalles()
{
    if (!corredorPendiente)
        return;

    corredorCalles = QPainterPath();
    corredorCalles.setFillRule(Qt::WindingFill);
    for (int i = 0; i < tramosPorArista.size(); ++i)
    {
        const TramoVisible &tramo = tramosPorArista[i];
        if (tramo.esVisible())
            corredorCalles.addPath(corredorDeTramo(tramo.getEje().p1(), tramo.getEje().p2(),
                                                   anchoConAcera(tramo.esAvenida()) + 4.0));
    }
    QPainterPath superficieMapa;
    superficieMapa.addRect(sim->getLimitesMapa());
    terrenoDisponible = superficieMapa.subtracted(corredorCalles);
    QPainterPath agua;
    agua.addPolygon(sim->getPoligonoAgua());
    agua.closeSubpath();
    plataformaPuentes = agua.intersected(corredorCalles);
    corredorPendiente = false;
}

void MapaWidget::dibujarEscenario(QPainter &p)
{
    QRectF b = sim->getLimitesMapa();
    QRectF ext = b.adjusted(-300.0, -120.0, 300.0, 120.0);
    QLinearGradient fondo(ext.topLeft(), ext.bottomRight());
    fondo.setColorAt(0.0, QColor("#303733"));
    fondo.setColorAt(1.0, QColor("#262e2c"));
    p.fillRect(ext, fondo);

    QPolygonF baseAgua = sim->getPoligonoAgua();
    if (baseAgua.size() >= 3)
    {
        QPolygonF orilla;
        orilla << QPointF(baseAgua[1].x() + 12.0, b.top() - 120.0);
        for (int i = 1; i <= baseAgua.size() - 2; i++)
        {
            orilla << baseAgua[i];
        }
        orilla << QPointF(baseAgua[baseAgua.size() - 2].x() + 35.0, b.bottom() + 120.0);

        QPolygonF agua;
        agua << QPointF(b.left() - 120.0, b.top() - 120.0);
        for (int i = 0; i < orilla.size(); i++)
        {
            agua << orilla[i];
        }
        agua << QPointF(b.left() - 120.0, b.bottom() + 120.0);

        QLinearGradient ribera(QPointF(b.left() - 95.0, b.top()), QPointF(b.left() + 235.0, b.top()));
        ribera.setColorAt(0.0, QColor(25, 50, 56, 0));
        ribera.setColorAt(0.18, QColor(25, 50, 56, 115));
        ribera.setColorAt(0.36, QColor("#193238"));
        ribera.setColorAt(1.0, QColor("#295055"));

        p.setPen(Qt::NoPen);
        p.setBrush(ribera);
        p.drawPolygon(agua);

        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#48625f"), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPolyline(orilla);
    }

    actualizarCorredorCalles();
    p.save();
    p.setClipPath(terrenoDisponible, Qt::IntersectClip);
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

        if (aeropuerto)
        {
            const double largo = interior.width() * 0.78;
            const QRectF pista(-largo / 2, -17, largo, 34);
            QPainterPath huella;
            huella.addRect(pista);
            QTransform rotacion;
            rotacion.rotate(-18);
            QPointF posicion;
            if (!ubicarHitoManzana(recorte, centro, rotacion.map(huella), posicion))
            {
                p.restore();
                continue;
            }
            p.save();
            p.translate(posicion);
            p.rotate(-18);
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
            QPainterPath huella;
            huella.addRoundedRect(QRectF(-54, -35, 108, 70), 35, 35);
            QPointF posicion;
            if (!ubicarHitoManzana(recorte, centro, huella, posicion))
            {
                p.restore();
                continue;
            }
            const QRectF gradas(posicion.x() - 54, posicion.y() - 35, 108, 70);
            p.setBrush(QColor("#62665a"));
            p.drawRoundedRect(gradas, 35, 35);
            p.setBrush(QColor("#263933"));
            p.drawRoundedRect(gradas.adjusted(9, 8, -9, -8), 26, 26);
            const QRectF cancha(posicion.x() - 33, posicion.y() - 18, 66, 36);
            p.setPen(QPen(QColor("#718575"), 1.4));
            p.setBrush(QColor("#48614f"));
            p.drawRect(cancha);
            p.drawLine(QPointF(posicion.x(), cancha.top()), QPointF(posicion.x(), cancha.bottom()));
            p.drawEllipse(posicion, 6, 6);
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
            const QPointF arboles[6] = {QPointF(-48, -26), QPointF(-32, -36), QPointF(-23, -15),
                                        QPointF(41, 25), QPointF(22, 36), QPointF(53, 35)};
            for (int k = 0; k < 6; ++k)
            {
                const QPointF punto = centro + arboles[k];
                if (corredorCalles.intersects(QRectF(punto - QPointF(10, 10), QSizeF(23, 24))))
                    continue;
                p.setBrush(QColor("#263d33"));
                p.drawEllipse(punto + QPointF(3, 4), 10, 10);
                p.setBrush(QColor("#526e59"));
                p.drawEllipse(punto, 9, 9);
            }
        }
        else
        {
            const double ancho = interior.width();
            const double alto = interior.height();
            if (industrial || terminal)
            {
                dibujarTechoManzana(p, QRectF(centro.x() - ancho * 0.30, centro.y() - alto * 0.31,
                                              ancho * 0.58, alto * 0.25), color.lighter(125));
                dibujarTechoManzana(p, QRectF(centro.x() - ancho * 0.16, centro.y() + alto * 0.09,
                                              ancho * 0.43, alto * 0.19), color.lighter(116));
            }
            else
            {
                const double signo = i % 2 == 0 ? 1.0 : -1.0;
                const QPointF principal = centro + QPointF(signo * ancho * 0.15, -alto * 0.13);
                dibujarTechoManzana(p, QRectF(principal.x() - ancho * 0.16, principal.y() - alto * 0.12,
                                              ancho * 0.32, alto * 0.24), color.lighter(116));
                dibujarTechoManzana(p, QRectF(centro.x() - signo * ancho * 0.23 - 13, centro.y() + alto * 0.09,
                                              26, alto * 0.25), color.lighter(112));
                if (i % 3 == 0)
                    dibujarTechoManzana(p, QRectF(centro.x() + signo * ancho * 0.11 - 13, centro.y() + alto * 0.17,
                                                  26, alto * 0.17), color.lighter(112));
            }
        }
        p.restore();
    }
    p.restore();
}

void MapaWidget::dibujarVistaPreviaEdicion(QPainter &p)
{
    if (modoConducir || (herramientaEdicion != 1 && herramientaEdicion != 2)
        || (!cursorEnMapa && destinoProvisional < 0))
        return;

    const Grafo *grafo = sim->getGrafo();
    const QRectF limites = sim->getLimitesMapa();

    p.save();
    if (herramientaEdicion == 1 && limites.contains(posicionCursorMapa))
    {
        if (const Nodo *nodo = grafo->buscarNodo(nodoBajoMouse))
        {
            dibujarRotuloEdicion(p, nodo->getPosicion() - QPointF(0, 38),
                                 QStringLiteral("Cruce existente"), textoSecundario);
        }
        else
        {
            const RevisionNodo revision = grafo->previsualizarNodo(posicionCursorMapa);
            const QPointF posicion = revision.esValida() ? revision.getPosicion() : posicionCursorMapa;
            const QColor acento = revision.esValida() ? colorSeleccion : colorEdicionInvalida;
            QColor relleno = acento;
            relleno.setAlpha(62);
            p.setBrush(relleno);
            p.setPen(QPen(acento, 3.0, Qt::DashLine));
            p.drawEllipse(posicion, 15.0, 15.0);
            p.setPen(Qt::NoPen);
            p.setBrush(acento);
            p.drawEllipse(posicion, 4.5, 4.5);
            if (revision.getDivideCalle())
            {
                p.setPen(QPen(acento, 1.5, Qt::DotLine));
                p.drawLine(posicionCursorMapa, posicion);
            }
            const QString texto = revision.esValida()
                ? (revision.getDivideCalle() ? QStringLiteral("Crear cruce aquí") : QStringLiteral("Agregar nodo aquí"))
                : revision.getMotivo();
            dibujarRotuloEdicion(p, posicion - QPointF(0, 39), texto, acento);
        }
    }
    else if (herramientaEdicion == 2)
    {
        const Nodo *origen = grafo->buscarNodo(nodoPendiente);
        const int idDestino = destinoProvisional >= 0 ? destinoProvisional : nodoBajoMouse;
        const Nodo *destino = grafo->buscarNodo(idDestino);
        if (origen != nullptr && (destino != nullptr || limites.contains(posicionCursorMapa)))
        {
            const QPointF desde = origen->getPosicion();
            const QPointF hasta = destino != nullptr ? destino->getPosicion() : posicionCursorMapa;
            const QLineF linea(desde, hasta);
            const RevisionCalle revision = destino != nullptr
                ? grafo->previsualizarCalle(nodoPendiente, idDestino, calleDobleSentido) : RevisionCalle();
            const QColor acento = destino == nullptr ? textoSecundario
                : revision.esValida() ? colorSeleccion : colorEdicionInvalida;
            QColor relleno = acento;
            relleno.setAlpha(revision.esValida() ? 74 : 38);
            if (linea.length() > 1.0)
            {
                p.fillPath(corredorDeTramo(desde, hasta, anchoConAcera(false) + 4.0), relleno);
                p.setPen(QPen(acento, revision.esValida() ? 3.0 : 2.0, Qt::DashLine, Qt::RoundCap));
                p.drawLine(linea);
            }
            if (revision.esValida() && linea.length() >= 30.0)
            {
                const QPointF direccion = (hasta - desde) / linea.length();
                p.setPen(QPen(acento, 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                dibujarFlechaPrevia(p, linea.pointAt(calleDobleSentido ? 0.66 : 0.55), direccion);
                if (calleDobleSentido)
                    dibujarFlechaPrevia(p, linea.pointAt(0.34), -direccion);
            }
            const QVector<QPointF> &cruces = revision.getCruces();
            for (int i = 0; i < cruces.size(); ++i)
            {
                const QPointF &cruce = cruces[i];
                p.setPen(QPen(colorManual, 3.0));
                p.setBrush(fondoPanel);
                p.drawEllipse(cruce, 12.0, 12.0);
                p.setPen(QPen(colorManual, 2.0));
                p.drawLine(cruce - QPointF(5, 0), cruce + QPointF(5, 0));
                p.drawLine(cruce - QPointF(0, 5), cruce + QPointF(0, 5));
            }
            if (destino != nullptr)
            {
                const QLocale locale(QLocale::Spanish, QLocale::Colombia);
                const QString texto = revision.esValida()
                    ? QStringLiteral("%1 · peso %2%3")
                          .arg(calleDobleSentido ? QStringLiteral("Doble sentido") : QStringLiteral("Sentido único"),
                               locale.toString(revision.getPeso(), 'f', 1),
                               cruces.isEmpty()
                                   ? QString()
                                   : QStringLiteral(" · %1 %2").arg(cruces.size()).arg(
                                         cruces.size() == 1 ? QStringLiteral("cruce") : QStringLiteral("cruces")))
                    : revision.getMotivo();
                const QPointF ancla = cruces.isEmpty() ? linea.center() : cruces.first();
                dibujarRotuloEdicion(p, ancla - QPointF(0, cruces.isEmpty() ? 27 : 39), texto, acento);
            }
        }
    }
    p.restore();
}

void MapaWidget::dibujarConfirmacion(QPainter &p)
{
    if (animacionConfirmacion.state() != QVariantAnimation::Running)
        return;

    const double avance = animacionConfirmacion.currentValue().toDouble();
    QColor acento = colorSeleccion;
    acento.setAlphaF(0.95 * (1.0 - avance));
    p.save();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(acento, confirmarTramo ? 6.0 - 3.5 * avance : 3.0,
                  Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (confirmarTramo)
        p.drawLine(inicioConfirmacion, finConfirmacion);
    else
        p.drawEllipse(inicioConfirmacion, 17.0 + 26.0 * avance, 17.0 + 26.0 * avance);
    p.restore();
}

void MapaWidget::dibujarRecorridoConsultado(QPainter &p)
{
    if (!temporizadorRecorrido.isActive())
        return;

    const QRectF limites = sim->getLimitesMapa().intersected(p.clipBoundingRect()).adjusted(6, 6, -6, -6);
    const double escala = p.worldTransform().m11();
    const QFont fuente = fuentePixel(qBound(16, qRound(11.0 / escala), 27), true);
    const QFontMetricsF medidas(fuente);
    p.save();
    p.setFont(fuente);
    dibujarMarcaRecorrido(p, limites, medidas, destinoConsultado, QStringLiteral("Destino"), QColor("#d7c8ff"), false);
    dibujarMarcaRecorrido(p, limites, medidas, recogidaConsultada, QStringLiteral("Recogida"), colorSeleccion, true);
    p.restore();
}

void MapaWidget::dibujarCalles(QPainter &p, bool esMinimapa)
{
    const QPen acera(QColor("#565d56"), anchoConAcera(false), Qt::SolidLine, Qt::RoundCap);
    const QPen aceraAvenida(QColor("#565d56"), anchoConAcera(true), Qt::SolidLine, Qt::RoundCap);
    for (int i = 0; i < tramosPorArista.size(); ++i)
    {
        const TramoVisible &tramo = tramosPorArista[i];
        if (!tramo.esVisible())
            continue;
        p.setPen(tramo.esAvenida() ? aceraAvenida : acera);
        p.drawLine(tramo.getEje());
    }

    p.fillPath(plataformaPuentes, QColor("#818f8c"));
    const Grafo *grafo = sim->getGrafo();
    const ElementoArista *elemento = grafo->getAristas()->getPrimero();
    const QColor baseCalle("#373e3d");
    const QColor baseAvenida("#414746");
    for (int i = 0; i < tramosPorArista.size(); ++i)
    {
        const TramoVisible &tramo = tramosPorArista[i];
        Arista *arista = elemento->getArista();
        elemento = elemento->getSiguiente();
        if (!tramo.esVisible())
            continue;
        const QColor color = mezclarColores(tramo.esAvenida() ? baseAvenida : baseCalle, colorTrafico,
            qBound(0.0, (grafo->obtenerNivelCongestion(arista) - 1.05) * 0.14, 0.26));
        p.setPen(QPen(color, tramo.esAvenida() ? 35 : 24, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(tramo.getEje());
    }

    if (esMinimapa)
        return;

    QPen marcas(QColor(216, 212, 186, 95), 1.4, Qt::CustomDashLine, Qt::FlatCap);
    marcas.setDashPattern({6, 7});
    const QPen doble(QColor("#a5aaa1"), modoConducir && taxiEnFoco >= 0 ? 1.3 : 2.0,
                     Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    const QPen unico(QColor("#e8dfc9"), 2.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    for (int i = 0; i < tramosPorArista.size(); ++i)
    {
        const TramoVisible &tramo = tramosPorArista[i];
        if (!tramo.debeDibujarFlechas())
            continue;

        if (!tramo.getLineaCentral().isNull())
        {
            p.setPen(marcas);
            p.drawLine(tramo.getLineaCentral());
        }
        p.setPen(tramo.esDobleSentido() ? doble : unico);
        p.drawPolyline(tramo.getFlechaIda(), 3);
        if (tramo.esDobleSentido())
            p.drawPolyline(tramo.getFlechaVuelta(), 3);
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
    const Grafo *grafo = sim->getGrafo();
    const QFont fuente = fuentePixel(12, true);
    const QFontMetricsF fm(fuente);
    p.setFont(fuente);

    const ElementoArista *elemento = grafo->getAristas()->getPrimero();
    for (int i = 0; i < tramosPorArista.size(); ++i)
    {
        const TramoVisible &tramo = tramosPorArista[i];
        Arista *arista = elemento->getArista();
        elemento = elemento->getSiguiente();
        if (!tramo.esVisible())
            continue;

        const QPointF centro = tramo.getEje().center();
        const QString texto = QString::number(grafo->obtenerPesoEfectivoArista(arista), 'f', 1);
        const QColor tono = mezclarColores(textoPrincipal, colorTrafico,
                                           (grafo->obtenerNivelCongestion(arista) - 1.05) / 1.6);
        const double ancho = fm.horizontalAdvance(texto) + 10;
        const QRectF caja(centro.x() - ancho / 2, centro.y() - 8, ancho, 16);
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
    }

    if (esMinimapa)
    {
        return;
    }
    dibujarNombresNodos(p, destinoVisible);

    if (g->buscarNodo(nodoPendiente) != nullptr)
    {
        p.setPen(QPen(colorSeleccion, 3.5));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(g->buscarNodo(nodoPendiente)->getPosicion(), 25, 25);
    }
}

void MapaWidget::dibujarNombresNodos(QPainter &p, int destinoVisible)
{
    const double escala = p.worldTransform().m11();
    const QFont fuenteNormal = fuentePixel(qBound(14, qRound(11.0 / escala), 35));
    const QFont fuenteEspecial = fuentePixel(qBound(16, qRound(11.5 / escala), 36), true);
    if (escalaNombres != escala)
    {
        nombresNodos.clear();
        escalaNombres = escala;
        const QFontMetricsF medidasNormal(fuenteNormal);
        const QFontMetricsF medidasEspecial(fuenteEspecial);
        const QRectF mundo = modoConducir ? encuadreConduccion : sim->getLimitesMapa();
        const QRectF vista = p.clipBoundingRect().intersected(mundo).adjusted(4, 4, -4, -8);
        QVector<QRectF> ocupados;
        const ListaNodos *nodos = sim->getGrafo()->getNodos();
        nombresNodos.reserve(nodos->getCantidad());
        ocupados.reserve(nodos->getCantidad());

        for (const ElementoNodo *e = nodos->getPrimero(); e != nullptr; e = e->getSiguiente())
        {
            const Nodo *nodo = e->getNodo();
            if (nodo == nullptr)
                continue;
            const bool especial = nodo->getTipo() != "Normal";
            const QFontMetricsF &medidas = especial ? medidasEspecial : medidasNormal;
            QStaticText texto(medidas.elidedText(nodo->getNombre(), Qt::ElideRight, qRound(145.0 / escala)));
            texto.setTextFormat(Qt::PlainText);
            texto.prepare(p.worldTransform(), especial ? fuenteEspecial : fuenteNormal);
            const QSizeF tamano = texto.size();
            const QPointF pos = nodo->getPosicion();
            const double x = qBound(vista.left(), pos.x() - tamano.width() / 2,
                                    qMax(vista.left(), vista.right() - tamano.width()));
            const QRectF debajo(QPointF(x, pos.y() + (especial ? 22 : 18)), tamano);
            QRectF caja = debajo;
            const QPointF alternativas[4] = {
                debajo.topLeft(),
                QPointF(x, pos.y() - 43 - tamano.height()),
                pos + QPointF(22, 10),
                pos + QPointF(-22 - tamano.width(), 10)
            };
            for (int a = 0; a < 4; ++a)
            {
                const QRectF candidata(alternativas[a], tamano);
                if (!vista.contains(candidata))
                    continue;
                bool libre = true;
                for (int k = 0; k < ocupados.size(); ++k)
                {
                    if (ocupados[k].intersects(candidata))
                    {
                        libre = false;
                        break;
                    }
                }
                if (libre)
                {
                    caja = candidata;
                    break;
                }
            }
            ocupados.append(caja.adjusted(-3, -2, 3, 2));
            const QColor color = especial ? mezclarColores(obtenerColorTipoNodo(nodo->getTipo()), textoSecundario, 0.38)
                                          : QColor("#d0d8d4");
            nombresNodos.append(NombreNodo(nodo->getId(), caja.topLeft(), texto, color, especial));
        }
    }
    p.save();
    p.setPen(QColor("#151917"));
    const QPointF sombra(0.8 / escala, 1.15 / escala);
    for (int i = 0; i < nombresNodos.size(); ++i)
    {
        const NombreNodo &nombre = nombresNodos[i];
        p.setFont(nombre.esEspecial() ? fuenteEspecial : fuenteNormal);
        p.drawStaticText(nombre.getPosicion() + sombra, nombre.getTexto());
    }
    for (int i = 0; i < nombresNodos.size(); ++i)
    {
        const NombreNodo &nombre = nombresNodos[i];
        p.setFont(nombre.esEspecial() ? fuenteEspecial : fuenteNormal);
        p.setPen(nombre.getId() == nodoBajoMouse || nombre.getId() == destinoVisible ? textoPrincipal : nombre.getColor());
        p.drawStaticText(nombre.getPosicion(), nombre.getTexto());
    }
    p.restore();
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

    p.fillRect(sim->getLimitesMapa().adjusted(-600.0, -600.0, 600.0, 600.0),
               QColor(16, 22, 31, int(110 * oscuridad)));

    for (int i = 0; i < sim->getCantidadTaxis(); i++)
    {
        Taxi *taxi = sim->getTaxi(i);
        p.save();
        p.translate(posicionDibujoTaxi(taxi));
        p.rotate(taxi->getAngulo());
        QLinearGradient haz(10, 0, 70, 0);
        const int intensidad = taxi->esManual() ? 170 : mostrarRutaDelTaxi(taxi) ? 85 : 43;
        haz.setColorAt(0.0, QColor(255, 244, 200, int(intensidad * oscuridad)));
        haz.setColorAt(1.0, QColor(255, 244, 200, 0));
        p.setPen(Qt::NoPen);
        p.setBrush(haz);
        QPolygonF cono;
        cono << QPointF(10, -5) << QPointF(70, -28) << QPointF(70, 28) << QPointF(10, 5);
        p.drawPolygon(cono);
        p.restore();
    }
}

QRectF MapaWidget::areaMinimapa() const
{
    if (sim == nullptr || sim->getTaxiManual() == nullptr || taxiEnFoco >= 0
        || width() < 1000 || height() < 575)
        return {};
    return QRectF(18, height() - 114, 138, 96);
}

void MapaWidget::dibujarMinimapa(QPainter &p)
{
    const QRectF panel = areaMinimapa();
    if (panel.isEmpty())
        return;

    const QRectF vista = panel.adjusted(1, 19, -1, -1);

    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 25));
    p.drawRoundedRect(panel.adjusted(-2, 1, 2, 5), 10, 10);
    p.setBrush(QColor(0, 0, 0, 45));
    p.drawRoundedRect(panel.adjusted(-1, 1, 1, 3), 9, 9);
    p.setBrush(QColor(0, 0, 0, 65));
    p.drawRoundedRect(panel.translated(0, 2), 8, 8);

    p.setPen(QPen(bordePanel, 1));
    p.setBrush(fondoPanel);
    p.drawRoundedRect(panel, 8, 8);

    QPainterPath recorte;
    recorte.addRoundedRect(vista, 6, 6);
    p.setClipPath(recorte);
    p.fillRect(vista, fondoVista);

    QTransform t;
    t.translate(vista.center().x(), vista.center().y());
    t.scale(0.20, 0.20);
    const QPointF referencia = sim->getTaxiManual()->getPosicion();
    t.translate(-referencia.x(), -referencia.y());
    p.setTransform(t);
    dibujarMundo(p, true);
    p.restore();

    p.setFont(fuentePixel(10, true));
    p.setPen(textoSecundario);
    p.drawText(panel.adjusted(10, 1, -10, -77), Qt::AlignVCenter | Qt::AlignLeft,
               QStringLiteral("Minimapa"));
}

QRectF MapaWidget::areaTaximetro() const
{
    if (sim == nullptr || !modoConducir || sim->getTaxiManual() == nullptr)
        return {};
    const TaxiManual *taxi = sim->getTaxiManual();
    const bool activo = (taxi != nullptr && taxi->getLlevaPasajero()) || (tiempoPausaCobro > 0.0);
    const double ancho = qMin(168.0, width() - 36.0);
    const double alto = activo ? 64.0 : 46.0;
    return QRectF(18.0, 18.0, ancho, alto);
}

void MapaWidget::dibujarTaximetro(QPainter &p)
{
    const QRectF panel = areaTaximetro();
    if (panel.isEmpty())
        return;

    const TaxiManual *taxi = sim->getTaxiManual();
    const bool ocupado = (taxi != nullptr && taxi->getLlevaPasajero());
    const bool enPausaCobro = (tiempoPausaCobro > 0.0);
    const QRectF interior = panel.adjusted(10, 6, -10, -6);

    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 25));
    p.drawRoundedRect(panel.adjusted(-2, 1, 2, 5), 10, 10);
    p.setBrush(QColor(0, 0, 0, 45));
    p.drawRoundedRect(panel.adjusted(-1, 1, 1, 3), 9, 9);
    p.setBrush(QColor(0, 0, 0, 65));
    p.drawRoundedRect(panel.translated(0, 2), 8, 8);

    p.setPen(QPen(enPausaCobro ? QColor("#ffd60a") : bordePanel, 1));
    p.setBrush(QColor("#1b222a"));
    p.drawRoundedRect(panel, 8, 8);

    p.setFont(fuentePixel(10, true));
    if (enPausaCobro)
    {
        p.setPen(QColor("#ffd60a"));
        p.drawText(QRectF(interior.x(), interior.y(), interior.width(), 13),
                   Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("COBRO TOTAL · VIAJE"));
        p.setPen(QColor("#4ade80"));
        p.drawText(QRectF(interior.x(), interior.y(), interior.width(), 13),
                   Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("Acreditado"));
    }
    else if (ocupado)
    {
        p.setPen(textoSecundario);
        p.drawText(QRectF(interior.x(), interior.y(), interior.width(), 13),
                   Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("TAXÍMETRO · EN RUTA"));
        const QString estado = sim->estaPausada() ? QStringLiteral("Pausa") : QStringLiteral("En vivo");
        p.setPen(sim->estaPausada() ? QColor("#f87171") : QColor("#38bdf8"));
        p.drawText(QRectF(interior.x(), interior.y(), interior.width(), 13),
                   Qt::AlignRight | Qt::AlignVCenter, estado);
    }
    else
    {
        p.setPen(textoSecundario);
        p.drawText(QRectF(interior.x(), interior.y(), interior.width(), 13),
                   Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("TAXÍMETRO · TU TAXI"));
        p.setPen(QColor("#94a3b8"));
        p.drawText(QRectF(interior.x(), interior.y(), interior.width(), 13),
                   Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("Libre"));
    }

    QFont numeros(QStringLiteral("Consolas"));
    numeros.setPixelSize(18);
    numeros.setBold(true);
    p.setFont(numeros);

    if (enPausaCobro)
    {
        p.setPen(QColor("#ffd60a"));
        p.drawText(QRectF(interior.x(), interior.y() + 14, interior.width(), 22),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   QStringLiteral("+") + formatearPesos(tarifaCobradaReciente));
    }
    else if (ocupado)
    {
        p.setPen(colorManual);
        p.drawText(QRectF(interior.x(), interior.y() + 14, interior.width(), 22),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   formatearPesos(taxi->getValorTaximetro()));
    }
    else
    {
        p.setPen(QColor("#64748b"));
        p.drawText(QRectF(interior.x(), interior.y() + 14, interior.width(), 22),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   formatearPesos(0.0));
    }

    if (enPausaCobro)
    {
        p.setFont(fuentePixel(9));
        p.setPen(QColor("#89e3be"));
        p.drawText(QRectF(interior.x(), interior.y() + 36, interior.width(), 14),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   QStringLiteral("Suma a tus ingresos totales"));
    }
    else if (ocupado)
    {
        const Nodo *nodo = sim->getGrafo()->buscarNodo(taxi->getDestinoPasajeroABordo());
        const QString nombreNodo = nodo != nullptr ? nodo->getNombre() : QStringLiteral("Desconocido");
        QString textoLinea;
        if (taxi->getRecargosCarrera() > 0.0)
        {
            textoLinea = QStringLiteral("+%1 rec. · %2").arg(formatearPesos(taxi->getRecargosCarrera()), nombreNodo);
        }
        else
        {
            textoLinea = QStringLiteral("Destino: %1").arg(nombreNodo);
        }

        p.setFont(fuentePixel(9));
        p.setPen(textoPrincipal);
        const QFontMetricsF medidas(p.font());
        p.drawText(QRectF(interior.x(), interior.y() + 36, interior.width(), 14),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   medidas.elidedText(textoLinea, Qt::ElideRight, qFloor(interior.width())));
    }

    p.restore();
}

void MapaWidget::dibujarAnimacionCobro(QPainter &p)
{
    if (!animCobroActiva)
        return;

    const double t = qBound(0.0, animCobroProgreso, 1.0);
    const double ease = std::sin(t * 3.14159265358979323846 / 2.0);
    const double arcoY = -35.0 * std::sin(t * 3.14159265358979323846);
    const QPointF posActual(
        (1.0 - ease) * animCobroInicio.x() + ease * animCobroFin.x(),
        (1.0 - ease) * animCobroInicio.y() + ease * animCobroFin.y() + arcoY
    );

    double alpha = 1.0;
    if (t < 0.15)
        alpha = t / 0.15;
    else if (t > 0.65)
        alpha = (1.0 - t) / 0.35;
    const int a = qBound(0, int(alpha * 255.0), 255);

    p.save();
    QFont f(QStringLiteral("Consolas"));
    f.setPixelSize(12);
    f.setBold(true);
    p.setFont(f);

    QFontMetrics fm(f);
    int anchoTexto = fm.horizontalAdvance(animCobroTexto);
    QRectF badge(posActual.x() - anchoTexto / 2.0 - 10, posActual.y() - 12, anchoTexto + 20, 24);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, qMin(75, a / 2)));
    p.drawRoundedRect(badge.translated(0, 2), 6, 6);

    p.setPen(QPen(QColor(255, 214, 10, a), 1.5));
    p.setBrush(QColor(18, 24, 34, qMin(240, a)));
    p.drawRoundedRect(badge, 6, 6);

    p.setPen(QColor(255, 214, 10, a));
    p.drawText(badge, Qt::AlignCenter, animCobroTexto);

    p.restore();
}

void MapaWidget::dibujarMarcoEsquinas(QPainter &p, QRectF marco) const
{
    p.save();
    p.setBrush(Qt::NoBrush);

    p.setPen(QPen(QColor(19, 24, 27, 175), 2.5));
    p.drawRoundedRect(marco.adjusted(1.0, 1.0, -1.0, -1.0), 7, 7);

    p.setPen(QPen(bordePanel, 1.2));
    p.drawRoundedRect(marco, 7, 7);

    const double largo = 18.0;
    const double r = 7.0;
    const QColor colorEsquina = modoConducir ? QColor("#677779") : QColor("#a8915f");
    p.setPen(QPen(colorEsquina, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    QPainterPath supIzq(QPointF(marco.left(), marco.top() + largo));
    supIzq.lineTo(marco.left(), marco.top() + r);
    supIzq.quadTo(marco.topLeft(), QPointF(marco.left() + r, marco.top()));
    supIzq.lineTo(marco.left() + largo, marco.top());
    p.drawPath(supIzq);

    QPainterPath supDer(QPointF(marco.right() - largo, marco.top()));
    supDer.lineTo(marco.right() - r, marco.top());
    supDer.quadTo(marco.topRight(), QPointF(marco.right(), marco.top() + r));
    supDer.lineTo(marco.right(), marco.top() + largo);
    p.drawPath(supDer);

    QPainterPath infIzq(QPointF(marco.left(), marco.bottom() - largo));
    infIzq.lineTo(marco.left(), marco.bottom() - r);
    infIzq.quadTo(marco.bottomLeft(), QPointF(marco.left() + r, marco.bottom()));
    infIzq.lineTo(marco.left() + largo, marco.bottom());
    p.drawPath(infIzq);

    QPainterPath infDer(QPointF(marco.right() - largo, marco.bottom()));
    infDer.lineTo(marco.right() - r, marco.bottom());
    infDer.quadTo(marco.bottomRight(), QPointF(marco.right(), marco.bottom() - r));
    infDer.lineTo(marco.right(), marco.bottom() - largo);
    p.drawPath(infDer);

    p.restore();
}

void MapaWidget::enCuadro(double dt)
{
    if (sim == nullptr)
        return;
    sincronizarRevisionGrafo();

    const double meta = sim->getReloj().oscuridad();
    const bool cambiaLuz = oscuridad != meta;
    const bool hayAviso = !textosFlotantes.isEmpty() || animCobroActiva || tiempoPausaCobro > 0.0;
    double factor = dt * 2.5;
    if (factor > 1.0) factor = 1.0;
    oscuridad += (meta - oscuridad) * factor;
    if (qAbs(meta - oscuridad) < 0.001)
        oscuridad = meta;

    if (tiempoPausaCobro > 0.0)
    {
        tiempoPausaCobro -= dt;
        if (tiempoPausaCobro < 0.0)
            tiempoPausaCobro = 0.0;
    }

    if (animCobroActiva)
    {
        animCobroProgreso += dt / animCobroDuracion;
        if (animCobroProgreso >= 1.0)
        {
            animCobroActiva = false;
            if (ventana != nullptr)
            {
                ventana->resaltarIngresoManual();
            }
        }
    }

    for (int i = textosFlotantes.size() - 1; i >= 0; i--)
    {
        textosFlotantes[i].avanzarEdad(dt);
        if (textosFlotantes[i].getEdad() > 2.2)
        {
            textosFlotantes.removeAt(i);
        }
    }
    if (!sim->estaPausada() || cambiaLuz || hayAviso)
        update();
}

void MapaWidget::enCarreraCompletada(int idTaxi, QPointF pos, double tarifa, QColor color)
{
    if (sim != nullptr && sim->getTaxiManual() != nullptr && sim->getTaxiManual()->getId() == idTaxi)
    {
        tiempoPausaCobro = 0.85;
        tarifaCobradaReciente = tarifa;

        const QRectF panel = areaTaximetro();
        animCobroActiva = true;
        animCobroProgreso = 0.0;
        animCobroDuracion = 0.55;
        animCobroInicio = panel.isValid() ? panel.center() : QPointF(100, 50);
        animCobroFin = QPointF(width() - 15, qMin(height() * 0.15, 80.0));
        animCobroTexto = QStringLiteral("+") + formatearPesos(tarifa);
    }

    if (taxiEnFoco >= 0 && taxiEnFoco != idTaxi)
        return;
    textosFlotantes.append(
        TextoFlotante(pos, QStringLiteral("Llegada · +") + formatearPesos(tarifa), color));
}

void MapaWidget::enPasajeroRecogido(int idTaxi, int origen, int)
{
    if (sim == nullptr || !modoConducir)
        return;
    const Taxi *manual = sim->getTaxiManual();
    const int taxiVisible = taxiEnFoco >= 0 ? taxiEnFoco : (manual != nullptr ? manual->getId() : -1);
    if (idTaxi == taxiVisible)
        resaltarNodo(origen);
}

void MapaWidget::enReinicioSimulacion()
{
    textosFlotantes.clear();
    tiempoPausaCobro = 0.0;
    animCobroActiva = false;
    animacionConfirmacion.stop();
    nodoPendiente = -1;
    destinoProvisional = -1;
    seleccionarNodoEnFoco(-1);
    invalidarGeometria();
    update();
}

void MapaWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    const QRectF exterior = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    p.setPen(QPen(QColor("#2e373d"), 1));
    p.setBrush(fondoPanel);
    p.drawRoundedRect(exterior, 8, 8);

    const QRectF marco = areaMarcoMapa();
    QPainterPath recorteMarco;
    recorteMarco.addRoundedRect(marco, 7, 7);
    p.save();
    p.setClipPath(recorteMarco);

    QLinearGradient fondoTerreno(marco.topLeft(), marco.bottomRight());
    fondoTerreno.setColorAt(0.0, QColor("#303733"));
    fondoTerreno.setColorAt(1.0, QColor("#262e2c"));
    p.fillRect(marco, fondoTerreno);

    if (sim == nullptr)
    {
        p.restore();
        dibujarMarcoEsquinas(p, marco);
        return;
    }

    sincronizarRevisionGrafo();

    const QTransform vista = transformacionVista();
    p.save();
    p.setTransform(vista);
    dibujarMundo(p, false);
    p.restore();

    dibujarMinimapa(p);
    dibujarTaximetro(p);
    dibujarAnimacionCobro(p);
    p.restore();

    dibujarMarcoEsquinas(p, marco);
}

void MapaWidget::mousePressEvent(QMouseEvent *e)
{
    if (sim == nullptr)
    {
        return;
    }
    if (e->button() == Qt::RightButton)
    {
        if (ventana != nullptr)
        {
            ventana->enCancelarClicado();
        }
        return;
    }
    if (e->button() != Qt::LeftButton || areaMinimapa().contains(e->position())
        || areaTaximetro().contains(e->position()))
    {
        return;
    }

    QPointF posMapa = transformacionVista().inverted().map(e->position());
    const int idNodo = nodoInteractivo(posMapa);
    if (idNodo >= 0)
    {
        if (ventana != nullptr)
        {
            ventana->enNodoClicado(idNodo);
        }
    }
    else if (sim->getLimitesMapa().contains(posMapa))
    {
        if (ventana != nullptr)
        {
            ventana->enEspacioVacioClicado(posMapa);
        }
    }
}

void MapaWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (sim == nullptr)
    {
        return;
    }
    if (areaMinimapa().contains(e->position()) || areaTaximetro().contains(e->position()))
    {
        const bool cursorEstabaEnMapa = cursorEnMapa;
        cursorEnMapa = false;
        seleccionarNodoEnFoco(-1);
        setCursor(Qt::ArrowCursor);
        const TaxiManual *taxi = sim->getTaxiManual();
        if (areaTaximetro().contains(e->position()) && taxi != nullptr && taxi->getLlevaPasajero())
        {
            const Nodo *destino = sim->getGrafo()->buscarNodo(taxi->getDestinoPasajeroABordo());
            setToolTip(destino != nullptr ? QStringLiteral("Destino · %1").arg(destino->getNombre())
                                         : QStringLiteral("Destino fuera del mapa"));
        }
        else
            setToolTip(QString());
        if (!modoConducir && cursorEstabaEnMapa)
            update();
        return;
    }
    if (!toolTip().isEmpty())
        setToolTip(QString());
    posicionCursorMapa = transformacionVista().inverted().map(e->position());
    cursorEnMapa = sim->getLimitesMapa().contains(posicionCursorMapa);
    seleccionarNodoEnFoco(nodoInteractivo(posicionCursorMapa));
    if (!modoConducir)
        update();
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
    setToolTip(QString());
    cursorEnMapa = false;
    seleccionarNodoEnFoco(-1);
    update();
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
    if (ventana != nullptr)
    {
        ventana->enNodoEnFocoCambiado(id);
    }
    update();
}

void MapaWidget::actualizarVista()
{
    update();
}

MapaWidget::MapaWidget(QWidget *parent) : QWidget(parent)
{
    sim = nullptr;
    ventana = nullptr;
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
    connect(&animacionNodo, &QVariantAnimation::valueChanged, this, &MapaWidget::actualizarVista);

    animacionConfirmacion.setDuration(480);
    animacionConfirmacion.setStartValue(0.0);
    animacionConfirmacion.setEndValue(1.0);
    animacionConfirmacion.setEasingCurve(QEasingCurve::OutCubic);
    connect(&animacionConfirmacion, &QVariantAnimation::valueChanged, this, &MapaWidget::actualizarVista);

    temporizadorRecorrido.setSingleShot(true);
    temporizadorRecorrido.setInterval(1900);
    connect(&temporizadorRecorrido, &QTimer::timeout, this, &MapaWidget::actualizarVista);
}

void MapaWidget::setVentanaPrincipal(MainWindow *nuevaVentana)
{
    ventana = nuevaVentana;
}

void MapaWidget::setSimulacion(Simulacion *nuevaSim)
{
    if (sim == nuevaSim)
        return;

    sim = nuevaSim;
    ultimaRevisionGrafo = (sim != nullptr && sim->getGrafo() != nullptr)
        ? sim->getGrafo()->getRevisionGrafo() : 0;
    invalidarGeometria();
    textosFlotantes.clear();
    tiempoPausaCobro = 0.0;
    animCobroActiva = false;
    animacionConfirmacion.stop();
    nodoPendiente = -1;
    destinoProvisional = -1;
    seleccionarNodoEnFoco(-1);
    update();
}

void MapaWidget::setNodoPendiente(int idNodo)
{
    nodoPendiente = idNodo;
    update();
}

void MapaWidget::setHerramientaEdicion(int modo)
{
    herramientaEdicion = modo;
    update();
}

void MapaWidget::setSentidoCalle(bool doble)
{
    calleDobleSentido = doble;
    update();
}

void MapaWidget::setDestinoProvisional(int idDestino)
{
    destinoProvisional = idDestino;
    update();
}

void MapaWidget::resaltarNodo(int idNodo)
{
    const Nodo *nodo = sim != nullptr ? sim->getGrafo()->buscarNodo(idNodo) : nullptr;
    if (nodo == nullptr)
        return;
    inicioConfirmacion = nodo->getPosicion();
    confirmarTramo = false;
    animacionConfirmacion.stop();
    animacionConfirmacion.start();
}

void MapaWidget::resaltarRecorridoPasajero(int origen, int destino)
{
    if (sim == nullptr || !modoConducir)
        return;

    const Nodo *recogida = sim->getGrafo()->buscarNodo(origen);
    const Nodo *llegada = sim->getGrafo()->buscarNodo(destino);
    if (recogida == nullptr || llegada == nullptr)
        return;

    recogidaConsultada = recogida->getPosicion();
    destinoConsultado = llegada->getPosicion();
    temporizadorRecorrido.start();
    update();
}

void MapaWidget::ocultarRecorridoPasajero()
{
    if (!temporizadorRecorrido.isActive())
        return;
    temporizadorRecorrido.stop();
    update();
}

void MapaWidget::resaltarCalle(int origen, int destino)
{
    if (sim == nullptr)
        return;
    const Nodo *desde = sim->getGrafo()->buscarNodo(origen);
    const Nodo *hasta = sim->getGrafo()->buscarNodo(destino);
    if (desde == nullptr || hasta == nullptr)
        return;
    inicioConfirmacion = desde->getPosicion();
    finConfirmacion = hasta->getPosicion();
    confirmarTramo = true;
    animacionConfirmacion.stop();
    animacionConfirmacion.start();
}

void MapaWidget::setModoConducir(bool activo)
{
    modoConducir = activo;
    escalaNombres = 0.0;
    if (activo)
        destinoProvisional = -1;
    else
        temporizadorRecorrido.stop();
    update();
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
