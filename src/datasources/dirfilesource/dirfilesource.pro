include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = kstdata_dirfilesource

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/dirfilesource/tmp

LIBS += -lgetdata++

SOURCES += \
    dirfilesource.cpp 

HEADERS += \
    dirfilesource.h 
