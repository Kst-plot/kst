include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_differentiation)

SOURCES += \
    differentiation.cpp

HEADERS += \
    differentiation.h

FORMS += differentiationconfig.ui
