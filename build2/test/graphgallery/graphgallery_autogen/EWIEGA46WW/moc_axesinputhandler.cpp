/****************************************************************************
** Meta object code from reading C++ file 'axesinputhandler.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../test/graphgallery/axesinputhandler.h"
#include <QtDataVisualization/q3dscene.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'axesinputhandler.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSAxesInputHandlerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSAxesInputHandlerENDCLASS = QtMocHelpers::stringData(
    "AxesInputHandler"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSAxesInputHandlerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject AxesInputHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<Q3DInputHandler::staticMetaObject>(),
    qt_meta_stringdata_CLASSAxesInputHandlerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSAxesInputHandlerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSAxesInputHandlerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AxesInputHandler, std::true_type>
    >,
    nullptr
} };

void AxesInputHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *AxesInputHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AxesInputHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSAxesInputHandlerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return Q3DInputHandler::qt_metacast(_clname);
}

int AxesInputHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3DInputHandler::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
