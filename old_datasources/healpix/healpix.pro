include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_healpix)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/fitsimage/tmp

LIBS += -lcfitsio

SOURCES += \
    healpix.cpp \
    healpix_tools_fits.cpp \
    healpix_tools_pix.cpp \
    healpix_tools_proj.cpp

HEADERS += \
    healpix.h \
    healpix_tools.h

FORMS += healpixconfig.ui
