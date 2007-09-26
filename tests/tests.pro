include($$PWD/../kst.pri)

QT += gui network svg xml opengl qt3support

CONFIG += qtestlib
TEMPLATE = app
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = runtests
DESTDIR = $$OUTPUT_DIR/bin

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$TOPLEVELDIR/src/libkstapp \
    $$OUTPUT_DIR/src/kst/tmp

LIBS += -lkst -lkstmath -lkstwidgets -lkstapp

SOURCES += \
    main.cpp \
    testdatasource.cpp \
    testvector.cpp \
    testscalar.cpp \
    testmatrix.cpp

HEADERS += \
    testdatasource.h \
    testvector.h \
    testscalar.h \
    testmatrix.h

QMAKE_RPATHDIR += $$OUTPUT_DIR/lib $$OUTPUT_DIR/plugin
