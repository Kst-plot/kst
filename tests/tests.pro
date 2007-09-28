include($$PWD/../kst.pri)

QT += gui network svg xml opengl qt3support

macx:CONFIG -= app_bundle
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
    testamatrix.cpp \
    testcsd.cpp \
    testdatasource.cpp \
    testeqparser.cpp \
    testhistogram.cpp \
    testlabelparser.cpp \
    testscalar.cpp \
    testmatrix.cpp \
    testpsd.cpp \
    testvector.cpp 

HEADERS += \
    testamatrix.h \
    testcsd.h \
    testdatasource.h \
    testhistogram.h \
    testeqparser.h \
    testlabelparser.h \
    testscalar.h \
    testmatrix.h \
    testpsd.h \
    testvector.h 

QMAKE_RPATHDIR += $$OUTPUT_DIR/lib $$OUTPUT_DIR/plugin
