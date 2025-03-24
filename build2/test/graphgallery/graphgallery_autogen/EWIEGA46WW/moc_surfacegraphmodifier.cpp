/****************************************************************************
** Meta object code from reading C++ file 'surfacegraphmodifier.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../test/graphgallery/surfacegraphmodifier.h"
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/qsurface3dseries.h>
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'surfacegraphmodifier.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSSurfaceGraphModifierENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSSurfaceGraphModifierENDCLASS = QtMocHelpers::stringData(
    "SurfaceGraphModifier",
    "enableSqrtSinModel",
    "",
    "enable",
    "enableHeightMapModel",
    "enableTopographyModel",
    "toggleItemOne",
    "show",
    "toggleItemTwo",
    "toggleItemThree",
    "toggleSeeThrough",
    "seethrough",
    "toggleOilHighlight",
    "highlight",
    "toggleShadows",
    "shadows",
    "toggleSurfaceTexture"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSSurfaceGraphModifierENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   74,    2, 0x0a,    1 /* Public */,
       4,    1,   77,    2, 0x0a,    3 /* Public */,
       5,    1,   80,    2, 0x0a,    5 /* Public */,
       6,    1,   83,    2, 0x0a,    7 /* Public */,
       8,    1,   86,    2, 0x0a,    9 /* Public */,
       9,    1,   89,    2, 0x0a,   11 /* Public */,
      10,    1,   92,    2, 0x0a,   13 /* Public */,
      12,    1,   95,    2, 0x0a,   15 /* Public */,
      14,    1,   98,    2, 0x0a,   17 /* Public */,
      16,    1,  101,    2, 0x0a,   19 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject SurfaceGraphModifier::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSSurfaceGraphModifierENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSSurfaceGraphModifierENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSSurfaceGraphModifierENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SurfaceGraphModifier, std::true_type>,
        // method 'enableSqrtSinModel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'enableHeightMapModel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'enableTopographyModel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'toggleItemOne'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'toggleItemTwo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'toggleItemThree'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'toggleSeeThrough'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'toggleOilHighlight'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'toggleShadows'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'toggleSurfaceTexture'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void SurfaceGraphModifier::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SurfaceGraphModifier *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->enableSqrtSinModel((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->enableHeightMapModel((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->enableTopographyModel((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->toggleItemOne((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->toggleItemTwo((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->toggleItemThree((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->toggleSeeThrough((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->toggleOilHighlight((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->toggleShadows((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->toggleSurfaceTexture((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *SurfaceGraphModifier::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SurfaceGraphModifier::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSSurfaceGraphModifierENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SurfaceGraphModifier::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
