include($$PWD/../../kst.pri)

TEMPLATE = app
TARGET = d2asc
DESTDIR = $$OUTPUT_DIR/bin

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$TOPLEVELDIR/src/libkstapp \
    $$OUTPUT_DIR/src/kst/tmp

win32:LIBS += -lkst2app -lkst2widgets -lkst2math -lkst2
!win32:LIBS += -lkst2 -lkst2math -lkst2widgets -lkst2app

SOURCES += \
    d2asc.cpp

QMAKE_RPATHDIR += $$OUTPUT_DIR/lib $$OUTPUT_DIR/plugin
