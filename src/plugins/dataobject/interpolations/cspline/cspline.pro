include(../../../plugins_subsub.pri)

TARGET = $$qtLibraryTarget(kstplugin_cspline)
LIBS += -lgsl

SOURCES += \
    cspline.cpp

HEADERS += \
    cspline.h

FORMS += csplineconfig.ui
