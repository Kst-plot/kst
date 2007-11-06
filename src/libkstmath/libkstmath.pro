include($$PWD/../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstmath
DESTDIR = $$OUTPUT_DIR/lib
win32:CONFIG += staticlib

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$OUTPUT_DIR/src/libkstmath/tmp

LIBS += -lkst

#Don't trigger qmake's lex/yacc handling by default.
#Rather we want to use the files that are pre-generated and checked in.
QMAKE_EXT_LEX = .lex
QMAKE_EXT_YACC = .yacc

SOURCES += \
    painter.cpp \
    colorsequence.cpp \
    settings.cpp \
    objectdefaults.cpp \
    labelparser.cpp \
    dataobject.cpp \
    dataobjectcollection.cpp \
    csd.cpp \
    csdfactory.cpp \
    psd.cpp \
    relation.cpp \
    curvepointsymbol.cpp \
    linestyle.cpp \
    curve.cpp \
    image.cpp \
    defaultnames.cpp \
    histogram.cpp \
    histogramfactory.cpp \
    equation.cpp \
    curvehint.cpp \
    basicplugin.cpp \
    fftsg_h.c \
    enodes.cpp \
    enodefactory.cpp \
    dialoglauncher.cpp \
    eparse-eh.cpp \
    eparse.cpp \
    palette.cpp \
    psdcalculator.cpp \
    escan.cpp \
    objectfactory.cpp \
    relationfactory.cpp \
    builtinobjects.cpp \
    equationfactory.cpp

HEADERS += \
    builtinobjects.h \
    dialoglauncher.h \
    enodefactory.h \
    enodes.h \
    eparse-eh.h \
    eparse.h \
    relation.h \
    basicplugin.h \
    colorsequence.h \
    csd.h \
    csdfactory.h \
    curvehint.h \
    curvepointsymbol.h \
    dataobjectcollection.h \
    dataobject.h \
    defaultnames.h \
    equation.h \
    histogram.h \
    histogramfactory.h \
    image.h \
    linestyle.h \
    objectdefaults.h \
    painter.h \
    palette.h \
    plotdefines.h \
    psd.h \
    settings.h \
    curve.h \
    labelparser.h \
    objectfactory.h \
    psdcalculator.h \
    relationfactory.h \
    equationfactory.h

#LEXSOURCES += escan.l
#YACCSOURCES += eparse.y
