/****************************************************************************
** Meta object code from reading C++ file 'OutputTableWidget.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../gui/include/OutputTableWidget.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OutputTableWidget.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_OutputTableWidget_t {
    QByteArrayData data[12];
    char stringdata0[104];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OutputTableWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OutputTableWidget_t qt_meta_stringdata_OutputTableWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "OutputTableWidget"
QT_MOC_LITERAL(1, 18, 18), // "exportCsvRequested"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 14), // "onEngineUpdate"
QT_MOC_LITERAL(4, 53, 6), // "size_t"
QT_MOC_LITERAL(5, 60, 4), // "tick"
QT_MOC_LITERAL(6, 65, 9), // "microtick"
QT_MOC_LITERAL(7, 75, 5), // "phase"
QT_MOC_LITERAL(8, 81, 3), // "rho"
QT_MOC_LITERAL(9, 85, 3), // "psi"
QT_MOC_LITERAL(10, 89, 7), // "mu_zero"
QT_MOC_LITERAL(11, 97, 6) // "forced"

    },
    "OutputTableWidget\0exportCsvRequested\0"
    "\0onEngineUpdate\0size_t\0tick\0microtick\0"
    "phase\0rho\0psi\0mu_zero\0forced"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OutputTableWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    7,   25,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4, QMetaType::Int, QMetaType::Char, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,    5,    6,    7,    8,    9,   10,   11,

       0        // eod
};

void OutputTableWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OutputTableWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->exportCsvRequested(); break;
        case 1: _t->onEngineUpdate((*reinterpret_cast< size_t(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< char(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6])),(*reinterpret_cast< bool(*)>(_a[7]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OutputTableWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&OutputTableWidget::exportCsvRequested)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject OutputTableWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_OutputTableWidget.data,
    qt_meta_data_OutputTableWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *OutputTableWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OutputTableWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OutputTableWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int OutputTableWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void OutputTableWidget::exportCsvRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
