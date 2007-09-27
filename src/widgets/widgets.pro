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
    kstwidgets.cpp \
    colorbutton.cpp \
    combobox.cpp \
    curveappearancewidget.cpp \
    curveplacementwidget.cpp \
    datarange.cpp \
    filerequester.cpp \
    gradienteditor.cpp \
    matrixselector.cpp \
    scalarselector.cpp \
    stringselector.cpp \
    vectorselector.cpp

HEADERS += \
    colorbutton.h \
    combobox.h \
    curveappearancewidget.h \
    curveplacementwidget.h \
    datarange.h \
    filerequester.h \
    gradienteditor.h \
    matrixselector.h \
    scalarselector.h \
    stringselector.h \
    vectorselector.h \
    kstwidgets.h

FORMS += \
    datarange.ui \
    curveappearancewidget.ui \
    curveplacementwidget.ui \
    matrixselector.ui \
    scalarselector.ui \
    stringselector.ui \
    vectorselector.ui

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc
