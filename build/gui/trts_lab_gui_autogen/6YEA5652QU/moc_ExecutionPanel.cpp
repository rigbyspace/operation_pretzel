/****************************************************************************
** Meta object code from reading C++ file 'ExecutionPanel.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../gui/include/ExecutionPanel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ExecutionPanel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ExecutionPanel_t {
    QByteArrayData data[16];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ExecutionPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ExecutionPanel_t qt_meta_stringdata_ExecutionPanel = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ExecutionPanel"
QT_MOC_LITERAL(1, 15, 12), // "runRequested"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 13), // "stopRequested"
QT_MOC_LITERAL(4, 43, 14), // "resetRequested"
QT_MOC_LITERAL(5, 58, 13), // "stepRequested"
QT_MOC_LITERAL(6, 72, 14), // "pauseRequested"
QT_MOC_LITERAL(7, 87, 16), // "updateFromEngine"
QT_MOC_LITERAL(8, 104, 6), // "size_t"
QT_MOC_LITERAL(9, 111, 4), // "tick"
QT_MOC_LITERAL(10, 116, 9), // "microtick"
QT_MOC_LITERAL(11, 126, 5), // "phase"
QT_MOC_LITERAL(12, 132, 3), // "rho"
QT_MOC_LITERAL(13, 136, 3), // "psi"
QT_MOC_LITERAL(14, 140, 7), // "mu_zero"
QT_MOC_LITERAL(15, 148, 6) // "forced"

    },
    "ExecutionPanel\0runRequested\0\0stopRequested\0"
    "resetRequested\0stepRequested\0"
    "pauseRequested\0updateFromEngine\0size_t\0"
    "tick\0microtick\0phase\0rho\0psi\0mu_zero\0"
    "forced"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ExecutionPanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,
       4,    0,   46,    2, 0x06 /* Public */,
       5,    0,   47,    2, 0x06 /* Public */,
       6,    0,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    7,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int, QMetaType::Char, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,    9,   10,   11,   12,   13,   14,   15,

       0        // eod
};

void ExecutionPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ExecutionPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->runRequested(); break;
        case 1: _t->stopRequested(); break;
        case 2: _t->resetRequested(); break;
        case 3: _t->stepRequested(); break;
        case 4: _t->pauseRequested(); break;
        case 5: _t->updateFromEngine((*reinterpret_cast< size_t(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< char(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6])),(*reinterpret_cast< bool(*)>(_a[7]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ExecutionPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ExecutionPanel::runRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ExecutionPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ExecutionPanel::stopRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ExecutionPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ExecutionPanel::resetRequested)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ExecutionPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ExecutionPanel::stepRequested)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ExecutionPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ExecutionPanel::pauseRequested)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ExecutionPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ExecutionPanel.data,
    qt_meta_data_ExecutionPanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ExecutionPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ExecutionPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ExecutionPanel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ExecutionPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ExecutionPanel::runRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ExecutionPanel::stopRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ExecutionPanel::resetRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ExecutionPanel::stepRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ExecutionPanel::pauseRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
