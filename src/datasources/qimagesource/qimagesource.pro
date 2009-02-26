include($$PWD/../../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
CONFIG += plugin
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstdata_qimagesource
DESTDIR = $$OUTPUT_DIR/plugin

INSTALL_DIR = $$(INSTDIR)
! isEmpty(INSTALL_DIR) {
  target.path = $$INSTALL_DIR/plugin
  INSTALLS += target
}

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$OUTPUT_DIR/src/datasources/qimagesource/tmp

LIBS += -L$$OUTPUT_DIR/lib -lkst

SOURCES += \
    qimagesource.cpp

HEADERS += \
    qimagesource.h