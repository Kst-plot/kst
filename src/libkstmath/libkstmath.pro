include($$PWD/../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstmath
DESTDIR = $$OUTPUT_DIR/lib

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
    kstpainter.cpp \
    kstcolorsequence.cpp \
    kstsettings.cpp \
    kstobjectdefaults.cpp \
    labelparser.cpp \
    kstdataobject.cpp \
    kstdataobjectcollection.cpp \
    kstcsd.cpp \
    kstpsd.cpp \
    kstrelation.cpp \
    kstcurvepointsymbol.cpp \
    kstlinestyle.cpp \
    kstvcurve.cpp \
    kstimage.cpp \
    kstdefaultnames.cpp \
    ksthistogram.cpp \
    kstequation.cpp \
    kstcurvehint.cpp \
    kstbasicplugin.cpp \
    fftsg_h.c \
    enodes.cpp \
    enodefactory.cpp \
    dialoglauncher.cpp \
    eparse-eh.cpp \
    eparse.c \
    psdcalculator.cpp \
    escan.c \
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
    kstrelation.h \
    kstbasicplugin.h \
    kstcolorsequence.h \
    kstcsd.h \
    kstcurvehint.h \
    kstcurvepointsymbol.h \
    kstdataobjectcollection.h \
    kstdataobject.h \
    kstdefaultnames.h \
    kstequation.h \
    ksthistogram.h \
    kstimage.h \
    kstlinestyle.h \
    kstobjectdefaults.h \
    kstpainter.h \
    kstplotdefines.h \
    kstpsd.h \
    kstsettings.h \
    kstvcurve.h \
    labelparser.h \
    objectfactory.h \
    psdcalculator.h \
    relationfactory.h \
    equationfactory.h
