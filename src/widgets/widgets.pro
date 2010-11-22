TOPOUT_REL=../..
include($$PWD/$$TOPOUT_REL/kst.pri)

QT += gui

TEMPLATE = lib
CONFIG += dll
DEFINES += BUILD_KSTWIDGETS
CONFIG += designer
TARGET = $$kstlib(kst2widgets)
DESTDIR = $$OUTPUT_DIR/lib
QT += core xml gui


!isEmpty(INSTALL_PREFIX) {
    target.path = $$INSTALL_PREFIX/$$INSTALL_LIBDIR
    INSTALLS += target
}
INCLUDEPATH += tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$OUTPUT_DIR/src/widgets/tmp

macx {
	CONFIG += lib_bundle
	LIBS += -F$$OUTPUT_DIR/lib
	qtAddLibrary(kst2lib)
	qtAddLibrary(kst2math)
} else {
	LIBS += \
		-L$$OUTPUT_DIR/lib \
		-L$$OUTPUT_DIR/plugin \
		-l$$kstlib(kst2math) \
		-l$$kstlib(kst2lib)
}
SOURCES += \
    colorbutton.cpp \
    colorpalette.cpp \
    combobox.cpp \
    curveappearance.cpp \
    curveplacement.cpp \
    curveselector.cpp \
    datarange.cpp \
    datasourceselectordialog.cpp \
    datasourceselector.cpp \
    dialogdefaults.cpp \
    fftoptions.cpp \
    filerequester.cpp \
    gradienteditor.cpp \
    labelbuilder.cpp \
    labellineedit.cpp \
    matrixselector.cpp \
    scalarlistselector.cpp \
    scalarselector.cpp \
    stringselector.cpp \
    vectorselector.cpp \
    widgets.cpp

HEADERS += \
    colorbutton.h \
    colorpalette.h \
    combobox.h \
    curveappearance.h \
    curveplacement.h \
    curveselector.h \
    datarange.h \
    datasourceselectordialog.h \
    datasourceselector.h \
    dialogdefaults.h \
    fftoptions.h \
    filerequester.h \
    gradienteditor.h \
    labelbuilder.h \
    labellineedit.h \
    matrixselector.h \
    scalarselector.h \
    scalarlistselector.h \
    stringselector.h \
    vectorselector.h \
    widgets.h

FORMS += \
    colorpalette.ui \
    curveappearance.ui \
    curveplacement.ui \
    curveselector.ui \
    datarange.ui \
    fftoptions.ui \
    labelbuilder.ui \
    matrixselector.ui \
    scalarlistselector.ui \
    scalarselector.ui \
    stringselector.ui \
    vectorselector.ui

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc
