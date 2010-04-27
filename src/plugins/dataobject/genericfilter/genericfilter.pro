include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_genericfilter)

SOURCES += \
    genericfilter.cpp

HEADERS += \
    genericfilter.h \
    polynom.h \
    filter.h

FORMS += genericfilterconfig.ui
