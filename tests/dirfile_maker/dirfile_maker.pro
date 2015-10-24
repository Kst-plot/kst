TOPOUT_REL=../..
include($$PWD/$$TOPOUT_REL/kst.pri)

TEMPLATE = app 
TARGET = $$kstlib(dirfile_maker)
DESTDIR = $$OUTPUT_DIR/bin
CONFIG -= precompile_header windows
CONFIG += console

QT -= core xml gui

!isEmpty(INSTALL_PREFIX) {
  target.path = $$INSTALL_PREFIX/bin
  INSTALLS += target
}

INCLUDEPATH +=

LIBS += 

SOURCES += dirfile_maker.c


