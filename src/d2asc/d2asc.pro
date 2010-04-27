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
	
LIBS += \
		-L$$OUTPUT_DIR/lib \
		-L$$OUTPUT_DIR/plugin \
		-l$$kstlib(kst2app) \
		-l$$kstlib(kst2widgets) \
		-l$$kstlib(kst2math) \
		-l$$kstlib(kst2lib)

SOURCES += \
    d2asc.cpp

QMAKE_RPATHDIR += $$OUTPUT_DIR/lib $$OUTPUT_DIR/plugin
