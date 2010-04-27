include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_planckIDEF)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/fitsimage/tmp

LIBS += -lcfitsio

SOURCES += \
    planckIDEF.cpp

HEADERS += \
    planckIDEF.h

FORMS += planckIDEFconfig.ui
