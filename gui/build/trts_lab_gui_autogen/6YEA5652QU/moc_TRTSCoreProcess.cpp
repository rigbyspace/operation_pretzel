/****************************************************************************
** Meta object code from reading C++ file 'TRTSCoreProcess.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/TRTSCoreProcess.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TRTSCoreProcess.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TRTSCoreProcess_t {
    QByteArrayData data[14];
    char stringdata0[177];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TRTSCoreProcess_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TRTSCoreProcess_t qt_meta_stringdata_TRTSCoreProcess = {
    {
QT_MOC_LITERAL(0, 0, 15), // "TRTSCoreProcess"
QT_MOC_LITERAL(1, 16, 12), // "engineOutput"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 4), // "line"
QT_MOC_LITERAL(4, 35, 11), // "engineError"
QT_MOC_LITERAL(5, 47, 11), // "runFinished"
QT_MOC_LITERAL(6, 59, 8), // "exitCode"
QT_MOC_LITERAL(7, 68, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(8, 89, 6), // "status"
QT_MOC_LITERAL(9, 96, 13), // "statusMessage"
QT_MOC_LITERAL(10, 110, 7), // "message"
QT_MOC_LITERAL(11, 118, 15), // "handleReadyRead"
QT_MOC_LITERAL(12, 134, 20), // "handleReadyReadError"
QT_MOC_LITERAL(13, 155, 21) // "handleProcessFinished"

    },
    "TRTSCoreProcess\0engineOutput\0\0line\0"
    "engineError\0runFinished\0exitCode\0"
    "QProcess::ExitStatus\0status\0statusMessage\0"
    "message\0handleReadyRead\0handleReadyReadError\0"
    "handleProcessFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TRTSCoreProcess[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       5,    2,   55,    2, 0x06 /* Public */,
       9,    1,   60,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   63,    2, 0x08 /* Private */,
      12,    0,   64,    2, 0x08 /* Private */,
      13,    2,   65,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 7,    6,    8,
    QMetaType::Void, QMetaType::QString,   10,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 7,    6,    8,

       0        // eod
};

void TRTSCoreProcess::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TRTSCoreProcess *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->engineOutput((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->engineError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->runFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 3: _t->statusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->handleReadyRead(); break;
        case 5: _t->handleReadyReadError(); break;
        case 6: _t->handleProcessFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TRTSCoreProcess::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TRTSCoreProcess::engineOutput)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TRTSCoreProcess::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TRTSCoreProcess::engineError)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TRTSCoreProcess::*)(int , QProcess::ExitStatus );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TRTSCoreProcess::runFinished)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TRTSCoreProcess::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TRTSCoreProcess::statusMessage)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TRTSCoreProcess::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TRTSCoreProcess.data,
    qt_meta_data_TRTSCoreProcess,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TRTSCoreProcess::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TRTSCoreProcess::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TRTSCoreProcess.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TRTSCoreProcess::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void TRTSCoreProcess::engineOutput(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TRTSCoreProcess::engineError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TRTSCoreProcess::runFinished(int _t1, QProcess::ExitStatus _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TRTSCoreProcess::statusMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
