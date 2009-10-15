include($$PWD/../../kst.pri)

TEMPLATE = app
TARGET = kst2
DESTDIR = $$OUTPUT_DIR/bin

! isEmpty(INSTALL_PREFIX) {
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

win32:LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin -lkst2app -lkst2widgets -lkst2math -lkst2
!win32:LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin -lkst2 -lkst2math -lkst2widgets -lkst2app

SOURCES += \
    main.cpp

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc

win32:QT += svg opengl
win32:RC_FILE = kst_icon.rc
