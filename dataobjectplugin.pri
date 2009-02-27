QT += xml qt3support

TEMPLATE = lib
CONFIG += plugin
OBJECTS_DIR = tmp
MOC_DIR = tmp
DESTDIR = $$OUTPUT_DIR/plugin

INSTALL_DIR = $$(INSTDIR)
! isEmpty(INSTALL_DIR) {
  target.path = $$INSTALL_DIR/plugin
  INSTALLS += target
}

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$OUTPUT_DIR/src/widgets \

LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin -lkst -lkstmath -lkstwidgets -lkstapp
