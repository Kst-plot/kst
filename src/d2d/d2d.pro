include($$PWD/../../kst.pri)

TEMPLATE = app
TARGET = d2d
DESTDIR = $$OUTPUT_DIR/bin

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$TOPLEVELDIR/src/libkstapp \
    $$OUTPUT_DIR/src/kst/tmp

win32:LIBS += -lkstapp -lkstwidgets -lkstmath -lkst
!win32:LIBS += -lkst -lkstmath -lkstwidgets -lkstapp

SOURCES += \
    d2d.cpp

QMAKE_RPATHDIR += $$OUTPUT_DIR/lib $$OUTPUT_DIR/plugin
