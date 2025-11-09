/****************************************************************************
** Meta object code from reading C++ file 'FingerprintPanel.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../gui/include/FingerprintPanel.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FingerprintPanel.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FingerprintPanel_t {
    QByteArrayData data[9];
    char stringdata0[119];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FingerprintPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FingerprintPanel_t qt_meta_stringdata_FingerprintPanel = {
    {
QT_MOC_LITERAL(0, 0, 16), // "FingerprintPanel"
QT_MOC_LITERAL(1, 17, 18), // "exportSvgRequested"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 18), // "exportPngRequested"
QT_MOC_LITERAL(4, 56, 21), // "setFingerprintSummary"
QT_MOC_LITERAL(5, 78, 7), // "summary"
QT_MOC_LITERAL(6, 86, 19), // "setFingerprintImage"
QT_MOC_LITERAL(7, 106, 6), // "pixmap"
QT_MOC_LITERAL(8, 113, 5) // "clear"

    },
    "FingerprintPanel\0exportSvgRequested\0"
    "\0exportPngRequested\0setFingerprintSummary\0"
    "summary\0setFingerprintImage\0pixmap\0"
    "clear"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FingerprintPanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   41,    2, 0x0a /* Public */,
       6,    1,   44,    2, 0x0a /* Public */,
       8,    0,   47,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QPixmap,    7,
    QMetaType::Void,

       0        // eod
};

void FingerprintPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FingerprintPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->exportSvgRequested(); break;
        case 1: _t->exportPngRequested(); break;
        case 2: _t->setFingerprintSummary((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->setFingerprintImage((*reinterpret_cast< const QPixmap(*)>(_a[1]))); break;
        case 4: _t->clear(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FingerprintPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FingerprintPanel::exportSvgRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FingerprintPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FingerprintPanel::exportPngRequested)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FingerprintPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_FingerprintPanel.data,
    qt_meta_data_FingerprintPanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FingerprintPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FingerprintPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FingerprintPanel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int FingerprintPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void FingerprintPanel::exportSvgRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FingerprintPanel::exportPngRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
