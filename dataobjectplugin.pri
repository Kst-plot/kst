QT += xml qt3support

TEMPLATE = lib
CONFIG += plugin
OBJECTS_DIR = tmp
MOC_DIR = tmp
DESTDIR = $$OUTPUT_DIR/plugin

! isEmpty(INSTALL_PREFIX) {
  target.path = $$INSTALL_PREFIX/$$INSTALL_LIBDIR/kst
  INSTALLS += target
}

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$OUTPUT_DIR/src/widgets \

win32:LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin -lkst2app -lkst2widgets -lkst2math -lkst2
!win32:LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin -lkst2 -lkst2math -lkst2widgets -lkst2app

