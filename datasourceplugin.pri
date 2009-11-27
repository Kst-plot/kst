QT += xml 

TEMPLATE = lib
CONFIG += plugin
CONFIG -= precompile_header
!win32:OBJECTS_DIR = tmp
!win32:MOC_DIR = tmp
DESTDIR = $$OUTPUT_DIR/plugin

! isEmpty(INSTALL_PREFIX) {
  target.path = $$INSTALL_PREFIX/$$INSTALL_LIBDIR/kst
  INSTALLS += target
}

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst 

!win32:LIBS += -L$$OUTPUT_DIR/lib -lkst2lib
win32:LIBS += -L$$OUTPUT_DIR/lib -l$$qtLibraryTarget(kst2lib)
