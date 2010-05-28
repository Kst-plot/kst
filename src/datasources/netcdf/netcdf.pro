TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/config.pri)
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_netcdfsource)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/netcdf/tmp

win32{
	message(Using netCDF files from $$(NETCDFDIR).)
	INCLUDEPATH += $$(NETCDFDIR)/include
	LIBS += -L$$(NETCDFDIR)/lib
}

LIBS += -l$$kstlib(netcdf_c++) -l$$kstlib(netcdf) 

SOURCES += \
    kstnetcdf.cpp

HEADERS += \
    kstnetcdf.h
