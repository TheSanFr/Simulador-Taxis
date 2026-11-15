/****************************************************************************
** Meta object code from reading C++ file 'Simulacion.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../Simulacion.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Simulacion.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10SimulacionE_t {};
} // unnamed namespace

template <> constexpr inline auto Simulacion::qt_create_metaobjectdata<qt_meta_tag_ZN10SimulacionE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Simulacion",
        "cuadroActualizado",
        "",
        "dtReal",
        "muestraGrafico",
        "segundosSimulados",
        "reinicioCompletado",
        "pausaCambiada",
        "pausada",
        "mensajeEvento",
        "texto",
        "QColor",
        "color",
        "carreraCompletada",
        "idTaxi",
        "QPointF",
        "posMapa",
        "tarifa",
        "enTickReloj"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'cuadroActualizado'
        QtMocHelpers::SignalData<void(double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
        // Signal 'muestraGrafico'
        QtMocHelpers::SignalData<void(double)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 5 },
        }}),
        // Signal 'reinicioCompletado'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pausaCambiada'
        QtMocHelpers::SignalData<void(bool)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 8 },
        }}),
        // Signal 'mensajeEvento'
        QtMocHelpers::SignalData<void(QString, QColor)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { 0x80000000 | 11, 12 },
        }}),
        // Signal 'carreraCompletada'
        QtMocHelpers::SignalData<void(int, QPointF, double, QColor)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { 0x80000000 | 15, 16 }, { QMetaType::Double, 17 }, { 0x80000000 | 11, 12 },
        }}),
        // Slot 'enTickReloj'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Simulacion, qt_meta_tag_ZN10SimulacionE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Simulacion::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10SimulacionE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10SimulacionE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10SimulacionE_t>.metaTypes,
    nullptr
} };

void Simulacion::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Simulacion *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->cuadroActualizado((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->muestraGrafico((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->reinicioCompletado(); break;
        case 3: _t->pausaCambiada((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->mensajeEvento((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QColor>>(_a[2]))); break;
        case 5: _t->carreraCompletada((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QPointF>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QColor>>(_a[4]))); break;
        case 6: _t->enTickReloj(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Simulacion::*)(double )>(_a, &Simulacion::cuadroActualizado, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Simulacion::*)(double )>(_a, &Simulacion::muestraGrafico, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Simulacion::*)()>(_a, &Simulacion::reinicioCompletado, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Simulacion::*)(bool )>(_a, &Simulacion::pausaCambiada, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (Simulacion::*)(QString , QColor )>(_a, &Simulacion::mensajeEvento, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (Simulacion::*)(int , QPointF , double , QColor )>(_a, &Simulacion::carreraCompletada, 5))
            return;
    }
}

const QMetaObject *Simulacion::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Simulacion::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10SimulacionE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Simulacion::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Simulacion::cuadroActualizado(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void Simulacion::muestraGrafico(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void Simulacion::reinicioCompletado()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Simulacion::pausaCambiada(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void Simulacion::mensajeEvento(QString _t1, QColor _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void Simulacion::carreraCompletada(int _t1, QPointF _t2, double _t3, QColor _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2, _t3, _t4);
}
QT_WARNING_POP
