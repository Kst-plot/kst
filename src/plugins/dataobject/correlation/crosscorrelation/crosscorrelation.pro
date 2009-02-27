include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = kstplugin_crosscorrelation
LIBS += -lgsl

SOURCES += \
    crosscorrelation.cpp

HEADERS += \
    crosscorrelation.h

FORMS += crosscorrelationconfig.ui
