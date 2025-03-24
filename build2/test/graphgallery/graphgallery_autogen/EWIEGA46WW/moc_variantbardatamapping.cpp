/****************************************************************************
** Meta object code from reading C++ file 'variantbardatamapping.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../test/graphgallery/variantbardatamapping.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'variantbardatamapping.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.0. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSVariantBarDataMappingENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSVariantBarDataMappingENDCLASS = QtMocHelpers::stringData(
    "VariantBarDataMapping",
    "rowIndexChanged",
    "",
    "columnIndexChanged",
    "valueIndexChanged",
    "rowCategoriesChanged",
    "columnCategoriesChanged",
    "mappingChanged",
    "rowIndex",
    "columnIndex",
    "valueIndex",
    "rowCategories",
    "columnCategories"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSVariantBarDataMappingENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       5,   56, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x06,    6 /* Public */,
       3,    0,   51,    2, 0x06,    7 /* Public */,
       4,    0,   52,    2, 0x06,    8 /* Public */,
       5,    0,   53,    2, 0x06,    9 /* Public */,
       6,    0,   54,    2, 0x06,   10 /* Public */,
       7,    0,   55,    2, 0x06,   11 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       8, QMetaType::Int, 0x00015103, uint(0), 0,
       9, QMetaType::Int, 0x00015103, uint(1), 0,
      10, QMetaType::Int, 0x00015103, uint(2), 0,
      11, QMetaType::QStringList, 0x00015103, uint(3), 0,
      12, QMetaType::QStringList, 0x00015103, uint(4), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject VariantBarDataMapping::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSVariantBarDataMappingENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSVariantBarDataMappingENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSVariantBarDataMappingENDCLASS_t,
        // property 'rowIndex'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'columnIndex'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'valueIndex'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'rowCategories'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // property 'columnCategories'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<VariantBarDataMapping, std::true_type>,
        // method 'rowIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'columnIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'valueIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rowCategoriesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'columnCategoriesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'mappingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void VariantBarDataMapping::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VariantBarDataMapping *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->rowIndexChanged(); break;
        case 1: _t->columnIndexChanged(); break;
        case 2: _t->valueIndexChanged(); break;
        case 3: _t->rowCategoriesChanged(); break;
        case 4: _t->columnCategoriesChanged(); break;
        case 5: _t->mappingChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VariantBarDataMapping::*)();
            if (_t _q_method = &VariantBarDataMapping::rowIndexChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (VariantBarDataMapping::*)();
            if (_t _q_method = &VariantBarDataMapping::columnIndexChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (VariantBarDataMapping::*)();
            if (_t _q_method = &VariantBarDataMapping::valueIndexChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (VariantBarDataMapping::*)();
            if (_t _q_method = &VariantBarDataMapping::rowCategoriesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (VariantBarDataMapping::*)();
            if (_t _q_method = &VariantBarDataMapping::columnCategoriesChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (VariantBarDataMapping::*)();
            if (_t _q_method = &VariantBarDataMapping::mappingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<VariantBarDataMapping *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->rowIndex(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->columnIndex(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->valueIndex(); break;
        case 3: *reinterpret_cast< QStringList*>(_v) = _t->rowCategories(); break;
        case 4: *reinterpret_cast< QStringList*>(_v) = _t->columnCategories(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<VariantBarDataMapping *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setRowIndex(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setColumnIndex(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setValueIndex(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setRowCategories(*reinterpret_cast< QStringList*>(_v)); break;
        case 4: _t->setColumnCategories(*reinterpret_cast< QStringList*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
    (void)_a;
}

const QMetaObject *VariantBarDataMapping::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VariantBarDataMapping::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSVariantBarDataMappingENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int VariantBarDataMapping::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void VariantBarDataMapping::rowIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void VariantBarDataMapping::columnIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void VariantBarDataMapping::valueIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void VariantBarDataMapping::rowCategoriesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void VariantBarDataMapping::columnCategoriesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void VariantBarDataMapping::mappingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
