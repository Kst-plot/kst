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
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$TOPLEVELDIR/src/widgets \
    $$TOPOUTDIR/src/widgets

LIBS  += \
			-L$$OUTPUT_DIR/lib \
			-L$$OUTPUT_DIR/plugin \
			-l$$kstlib(kst2widgets) \
			-l$$kstlib(kst2math) \
			-l$$kstlib(kst2lib)


