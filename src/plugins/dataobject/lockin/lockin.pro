include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_lockin)

SOURCES += lockin.cpp

HEADERS += lockin.h iirfilter.h

FORMS += lockinconfig.ui
