include($$PWD/../kst.pri)

QT += gui network svg xml opengl qt3support

macx:CONFIG -= app_bundle
CONFIG += qtestlib
TEMPLATE = app
!win32:OBJECTS_DIR = tmp
!win32:MOC_DIR = tmp
TARGET = runtests
DESTDIR = $$OUTPUT_DIR/bin

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$TOPLEVELDIR/src/libkstapp \
    $$OUTPUT_DIR/src/kst/tmp

win32:LIBS += -l$$kstlib(kst2app) -l$$kstlib(kst2widgets) -l$$kstlib(kst2math) -l$$kstlib(kst2lib)
!win32:LIBS += -lkst2lib -lkst2math -lkst2widgets -lkst2app

SOURCES += \
    main.cpp \
    testeditablematrix.cpp \
    testcsd.cpp \
    testdatamatrix.cpp \
    testdatasource.cpp \
    testeqparser.cpp \
    testgeneratedmatrix.cpp \
    testhistogram.cpp \
    testlabelparser.cpp \
    testscalar.cpp \
    testmatrix.cpp \
    testpsd.cpp \
    testobjectstore.cpp \
    testvector.cpp 

HEADERS += \
    testeditablematrix.h \
    testcsd.h \
    testdatamatrix.h \
    testdatasource.h \
    testhistogram.h \
    testeqparser.h \
    testgeneratedmatrix.h \
    testlabelparser.h \
    testscalar.h \
    testmatrix.h \
    testpsd.h \
    testobjectstore.h \
    testvector.h
