/****************************************************************************
** Meta object code from reading C++ file 'scatterdatamodifier.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../test/graphgallery/scatterdatamodifier.h"
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/qscatter3dseries.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scatterdatamodifier.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSScatterDataModifierENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSScatterDataModifierENDCLASS = QtMocHelpers::stringData(
    "ScatterDataModifier",
    "backgroundEnabledChanged",
    "",
    "enabled",
    "gridEnabledChanged",
    "shadowQualityChanged",
    "quality",
    "setBackgroundEnabled",
    "setGridEnabled",
    "setSmoothDots",
    "smooth",
    "changePresetCamera",
    "toggleItemCount",
    "toggleRanges",
    "changeStyle",
    "style",
    "changeTheme",
    "theme",
    "changeShadowQuality",
    "shadowQualityUpdatedByVisual",
    "QAbstract3DGraph::ShadowQuality",
    "shadowQuality"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSScatterDataModifierENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   92,    2, 0x06,    1 /* Public */,
       4,    1,   95,    2, 0x06,    3 /* Public */,
       5,    1,   98,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    1,  101,    2, 0x0a,    7 /* Public */,
       8,    1,  104,    2, 0x0a,    9 /* Public */,
       9,    1,  107,    2, 0x0a,   11 /* Public */,
      11,    0,  110,    2, 0x0a,   13 /* Public */,
      12,    0,  111,    2, 0x0a,   14 /* Public */,
      13,    0,  112,    2, 0x0a,   15 /* Public */,
      14,    1,  113,    2, 0x0a,   16 /* Public */,
      16,    1,  116,    2, 0x0a,   18 /* Public */,
      18,    1,  119,    2, 0x0a,   20 /* Public */,
      19,    1,  122,    2, 0x0a,   22 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 20,   21,

       0        // eod
};

Q_CONSTINIT const QMetaObject ScatterDataModifier::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSScatterDataModifierENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSScatterDataModifierENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSScatterDataModifierENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScatterDataModifier, std::true_type>,
        // method 'backgroundEnabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'gridEnabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'shadowQualityChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setBackgroundEnabled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setGridEnabled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setSmoothDots'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'changePresetCamera'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleItemCount'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggleRanges'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'changeStyle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'changeTheme'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'changeShadowQuality'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'shadowQualityUpdatedByVisual'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstract3DGraph::ShadowQuality, std::false_type>
    >,
    nullptr
} };

void ScatterDataModifier::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ScatterDataModifier *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->backgroundEnabledChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->gridEnabledChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->shadowQualityChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->setBackgroundEnabled((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->setGridEnabled((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->setSmoothDots((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->changePresetCamera(); break;
        case 7: _t->toggleItemCount(); break;
        case 8: _t->toggleRanges(); break;
        case 9: _t->changeStyle((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->changeTheme((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->changeShadowQuality((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->shadowQualityUpdatedByVisual((*reinterpret_cast< std::add_pointer_t<QAbstract3DGraph::ShadowQuality>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ScatterDataModifier::*)(bool );
            if (_t _q_method = &ScatterDataModifier::backgroundEnabledChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ScatterDataModifier::*)(bool );
            if (_t _q_method = &ScatterDataModifier::gridEnabledChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ScatterDataModifier::*)(int );
            if (_t _q_method = &ScatterDataModifier::shadowQualityChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *ScatterDataModifier::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScatterDataModifier::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSScatterDataModifierENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ScatterDataModifier::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void ScatterDataModifier::backgroundEnabledChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScatterDataModifier::gridEnabledChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ScatterDataModifier::shadowQualityChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
