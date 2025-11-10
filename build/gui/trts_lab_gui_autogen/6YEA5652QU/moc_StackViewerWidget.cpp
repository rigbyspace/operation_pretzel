/****************************************************************************
** Meta object code from reading C++ file 'StackViewerWidget.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../gui/include/StackViewerWidget.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StackViewerWidget.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_StackViewerWidget_t {
    QByteArrayData data[11];
    char stringdata0[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_StackViewerWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_StackViewerWidget_t qt_meta_stringdata_StackViewerWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "StackViewerWidget"
QT_MOC_LITERAL(1, 18, 14), // "onEngineUpdate"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 6), // "size_t"
QT_MOC_LITERAL(4, 41, 4), // "tick"
QT_MOC_LITERAL(5, 46, 9), // "microtick"
QT_MOC_LITERAL(6, 56, 5), // "phase"
QT_MOC_LITERAL(7, 62, 3), // "rho"
QT_MOC_LITERAL(8, 66, 3), // "psi"
QT_MOC_LITERAL(9, 70, 7), // "mu_zero"
QT_MOC_LITERAL(10, 78, 6) // "forced"

    },
    "StackViewerWidget\0onEngineUpdate\0\0"
    "size_t\0tick\0microtick\0phase\0rho\0psi\0"
    "mu_zero\0forced"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StackViewerWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    7,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Char, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,    4,    5,    6,    7,    8,    9,   10,

       0        // eod
};

void StackViewerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<StackViewerWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onEngineUpdate((*reinterpret_cast< size_t(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< char(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6])),(*reinterpret_cast< bool(*)>(_a[7]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject StackViewerWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_StackViewerWidget.data,
    qt_meta_data_StackViewerWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *StackViewerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StackViewerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_StackViewerWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int StackViewerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
