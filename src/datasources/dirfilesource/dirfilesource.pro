include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = kst2data_dirfilesource

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/dirfilesource/tmp

LIBS += -lgetdata++

SOURCES += \
    dirfilesource.cpp 

HEADERS += \
    dirfilesource.h 
