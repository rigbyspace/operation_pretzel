/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../gui/include/MainWindow.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[27];
    char stringdata0[299];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 12), // "engineUpdate"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 6), // "size_t"
QT_MOC_LITERAL(4, 32, 4), // "tick"
QT_MOC_LITERAL(5, 37, 9), // "microtick"
QT_MOC_LITERAL(6, 47, 5), // "phase"
QT_MOC_LITERAL(7, 53, 3), // "rho"
QT_MOC_LITERAL(8, 57, 3), // "psi"
QT_MOC_LITERAL(9, 61, 7), // "mu_zero"
QT_MOC_LITERAL(10, 69, 6), // "forced"
QT_MOC_LITERAL(11, 76, 14), // "handleStartRun"
QT_MOC_LITERAL(12, 91, 13), // "handleStopRun"
QT_MOC_LITERAL(13, 105, 11), // "handleReset"
QT_MOC_LITERAL(14, 117, 17), // "handleClearOutput"
QT_MOC_LITERAL(15, 135, 25), // "handleLoadConfigRequested"
QT_MOC_LITERAL(16, 161, 18), // "handleEngineOutput"
QT_MOC_LITERAL(17, 180, 4), // "line"
QT_MOC_LITERAL(18, 185, 17), // "handleEngineError"
QT_MOC_LITERAL(19, 203, 17), // "handleRunFinished"
QT_MOC_LITERAL(20, 221, 8), // "exitCode"
QT_MOC_LITERAL(21, 230, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(22, 251, 6), // "status"
QT_MOC_LITERAL(23, 258, 14), // "appendLogEntry"
QT_MOC_LITERAL(24, 273, 7), // "columns"
QT_MOC_LITERAL(25, 281, 9), // "logStatus"
QT_MOC_LITERAL(26, 291, 7) // "message"

    },
    "MainWindow\0engineUpdate\0\0size_t\0tick\0"
    "microtick\0phase\0rho\0psi\0mu_zero\0forced\0"
    "handleStartRun\0handleStopRun\0handleReset\0"
    "handleClearOutput\0handleLoadConfigRequested\0"
    "handleEngineOutput\0line\0handleEngineError\0"
    "handleRunFinished\0exitCode\0"
    "QProcess::ExitStatus\0status\0appendLogEntry\0"
    "columns\0logStatus\0message"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    7,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   84,    2, 0x08 /* Private */,
      12,    0,   85,    2, 0x08 /* Private */,
      13,    0,   86,    2, 0x08 /* Private */,
      14,    0,   87,    2, 0x08 /* Private */,
      15,    0,   88,    2, 0x08 /* Private */,
      16,    1,   89,    2, 0x08 /* Private */,
      18,    1,   92,    2, 0x08 /* Private */,
      19,    2,   95,    2, 0x08 /* Private */,
      23,    1,  100,    2, 0x08 /* Private */,
      25,    1,  103,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Char, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool, QMetaType::Bool,    4,    5,    6,    7,    8,    9,   10,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 21,   20,   22,
    QMetaType::Void, QMetaType::QStringList,   24,
    QMetaType::Void, QMetaType::QString,   26,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->engineUpdate((*reinterpret_cast< size_t(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< char(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6])),(*reinterpret_cast< bool(*)>(_a[7]))); break;
        case 1: _t->handleStartRun(); break;
        case 2: _t->handleStopRun(); break;
        case 3: _t->handleReset(); break;
        case 4: _t->handleClearOutput(); break;
        case 5: _t->handleLoadConfigRequested(); break;
        case 6: _t->handleEngineOutput((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->handleEngineError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->handleRunFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 9: _t->appendLogEntry((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 10: _t->logStatus((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(size_t , int , char , bool , bool , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::engineUpdate)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::engineUpdate(size_t _t1, int _t2, char _t3, bool _t4, bool _t5, bool _t6, bool _t7)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
