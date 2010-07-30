TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_dirfilesource)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/dirfilesource/tmp

win32{
	message(Using GetData C++ binding files from $$(GETDATADIR).)
	INCLUDEPATH += $$(GETDATADIR)/include
	LIBS += -L$$(GETDATADIR)/lib
}

LIBS += -lgetdata++
win32:LIBS += -lgetdata

SOURCES += \
    dirfilesource.cpp

HEADERS += \
    dirfilesource.h
