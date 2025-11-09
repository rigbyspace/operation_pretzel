/****************************************************************************
** Meta object code from reading C++ file 'AnalyzerPanel.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/AnalyzerPanel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalyzerPanel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AnalyzerPanel_t {
    QByteArrayData data[10];
    char stringdata0[118];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AnalyzerPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AnalyzerPanel_t qt_meta_stringdata_AnalyzerPanel = {
    {
QT_MOC_LITERAL(0, 0, 13), // "AnalyzerPanel"
QT_MOC_LITERAL(1, 14, 16), // "analyzeRequested"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 16), // "setResultSummary"
QT_MOC_LITERAL(4, 49, 7), // "summary"
QT_MOC_LITERAL(5, 57, 17), // "setClassification"
QT_MOC_LITERAL(6, 75, 14), // "classification"
QT_MOC_LITERAL(7, 90, 9), // "appendLog"
QT_MOC_LITERAL(8, 100, 4), // "line"
QT_MOC_LITERAL(9, 105, 12) // "clearResults"

    },
    "AnalyzerPanel\0analyzeRequested\0\0"
    "setResultSummary\0summary\0setClassification\0"
    "classification\0appendLog\0line\0"
    "clearResults"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AnalyzerPanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   40,    2, 0x0a /* Public */,
       5,    1,   43,    2, 0x0a /* Public */,
       7,    1,   46,    2, 0x0a /* Public */,
       9,    0,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,

       0        // eod
};

void AnalyzerPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AnalyzerPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->analyzeRequested(); break;
        case 1: _t->setResultSummary((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->setClassification((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->appendLog((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->clearResults(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AnalyzerPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AnalyzerPanel::analyzeRequested)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AnalyzerPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_AnalyzerPanel.data,
    qt_meta_data_AnalyzerPanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AnalyzerPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AnalyzerPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AnalyzerPanel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int AnalyzerPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void AnalyzerPanel::analyzeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
