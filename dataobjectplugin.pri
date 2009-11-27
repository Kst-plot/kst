QT += xml 

TEMPLATE = lib
CONFIG += plugin
!win32:OBJECTS_DIR = tmp
!win32:MOC_DIR = tmp
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

win32:LIBS  += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin -l$$qtLibraryTarget(kst2app) -l$$qtLibraryTarget(kst2widgets) -l$$qtLibraryTarget(kst2math) -l$$qtLibraryTarget(kst2lib)
!win32:LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin -lkst2lib -lkst2math -lkst2widgets -lkst2app

