TOPOUT_REL=../..
include($$PWD/$$TOPOUT_REL/kst.pri)

TEMPLATE = app
TARGET = $$kstlib(kst2)
DESTDIR = $$OUTPUT_DIR/bin
CONFIG -= precompile_header

!isEmpty(INSTALL_PREFIX) {
  target.path = $$INSTALL_PREFIX/bin
  INSTALLS += target
}

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
    main.cpp

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc

win32:QT += svg opengl
win32:RC_FILE = kst_icon.rc
