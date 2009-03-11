include($$PWD/../../kst.pri)

TEMPLATE = app
TARGET = kst
DESTDIR = $$OUTPUT_DIR/bin

INSTALL_DIR = $$(INSTDIR)
! isEmpty(INSTALL_DIR) {
  target.path = $$INSTALL_DIR/bin
  INSTALLS += target
}

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$TOPLEVELDIR/src/libkstapp \
    $$OUTPUT_DIR/src/kst/tmp

LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin -lkst -lkstmath -lkstwidgets -lkstapp

SOURCES += \
    main.cpp

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc

win32:QT += svg opengl
