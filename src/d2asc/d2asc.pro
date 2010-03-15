TOPOUT_REL=../..
include($$PWD/$$TOPOUT_REL/kst.pri)

TEMPLATE = app
TARGET = $$kstlib(d2asc)
DESTDIR = $$OUTPUT_DIR/bin
CONFIG -= precompile_header

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
    d2asc.cpp

QMAKE_RPATHDIR += $$OUTPUT_DIR/lib $$OUTPUT_DIR/plugin
