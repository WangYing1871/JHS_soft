/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../gui/inc/mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSmainwindowENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSmainwindowENDCLASS = QtMocHelpers::stringData(
    "mainwindow",
    "start_custom",
    "",
    "test_plot",
    "choose_dat_file",
    "choose_wave_file",
    "unpack",
    "draw_wave",
    "quit",
    "abort",
    "stop_bit_set",
    "do_custom",
    "draw_wave_fft",
    "draw_wave_fitfun"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSmainwindowENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   87,    2, 0x0a,    2 /* Public */,
       4,    0,   88,    2, 0x0a,    3 /* Public */,
       5,    0,   89,    2, 0x0a,    4 /* Public */,
       6,    0,   90,    2, 0x0a,    5 /* Public */,
       7,    1,   91,    2, 0x0a,    6 /* Public */,
       8,    0,   94,    2, 0x0a,    8 /* Public */,
       9,    0,   95,    2, 0x0a,    9 /* Public */,
      10,    0,   96,    2, 0x0a,   10 /* Public */,
      11,    0,   97,    2, 0x0a,   11 /* Public */,
      12,    1,   98,    2, 0x0a,   12 /* Public */,
      13,    0,  101,    2, 0x0a,   14 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject mainwindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSmainwindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSmainwindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSmainwindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<mainwindow, std::true_type>,
        // method 'start_custom'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'test_plot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'choose_dat_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'choose_wave_file'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unpack'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'draw_wave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'quit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'abort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stop_bit_set'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'do_custom'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'draw_wave_fft'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString const &, std::false_type>,
        // method 'draw_wave_fitfun'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void mainwindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<mainwindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->start_custom(); break;
        case 1: _t->test_plot(); break;
        case 2: _t->choose_dat_file(); break;
        case 3: _t->choose_wave_file(); break;
        case 4: _t->unpack(); break;
        case 5: _t->draw_wave((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->quit(); break;
        case 7: _t->abort(); break;
        case 8: _t->stop_bit_set(); break;
        case 9: _t->do_custom(); break;
        case 10: _t->draw_wave_fft((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->draw_wave_fitfun(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (mainwindow::*)();
            if (_t _q_method = &mainwindow::start_custom; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *mainwindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mainwindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSmainwindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int mainwindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void mainwindow::start_custom()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
