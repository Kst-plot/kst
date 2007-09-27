include($$PWD/../../kst.pri)

QT += gui xml qt3support

TEMPLATE = lib
CONFIG += designer plugin
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstwidgets

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target

DESTDIR = $$OUTPUT_DIR/plugin

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$TOPLEVELDIR/src/libkstmath \
    $$OUTPUT_DIR/src/widgets/tmp

SOURCES += \
    colorbutton.cpp \
    combobox.cpp \
    curveappearance.cpp \
    curveplacement.cpp \
    datarange.cpp \
    filerequester.cpp \
    gradienteditor.cpp \
    matrixselector.cpp \
    scalarselector.cpp \
    stringselector.cpp \
    vectorselector.cpp \
    widgets.cpp

HEADERS += \
    colorbutton.h \
    combobox.h \
    curveappearance.h \
    curveplacement.h \
    datarange.h \
    filerequester.h \
    gradienteditor.h \
    matrixselector.h \
    scalarselector.h \
    stringselector.h \
    vectorselector.h \
    widgets.h

FORMS += \
    datarange.ui \
    curveappearance.ui \
    curveplacement.ui \
    matrixselector.ui \
    scalarselector.ui \
    stringselector.ui \
    vectorselector.ui

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc
