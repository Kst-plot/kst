include(../../plugins_sub.pri)

TARGET = $$qtLibraryTarget(kstplugin_genericfilter)

SOURCES += \
    genericfilter.cpp

HEADERS += \
    genericfilter.h \
    polynom.h \
    filter.h

FORMS += genericfilterconfig.ui
