include($$PWD/../../kst.pri)

QT += gui xml

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
    datarange.cpp \
    editmultiplewidget.cpp \
    filerequester.cpp \
    gradienteditor.cpp

HEADERS += \
    colorbutton.h \
    combobox.h \
    datarange.h \
    editmultiplewidget.h \
    filerequester.h \
    gradienteditor.h \
    kstwidgets.h

FORMS += \
    datarange.ui \
    editmultiplewidget.ui

RESOURCES += \
    $$TOPLEVELDIR/src/images/images.qrc
