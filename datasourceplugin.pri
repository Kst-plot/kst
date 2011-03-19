include(config.pri)

QT += xml 

TEMPLATE = lib
CONFIG += plugin warn_on
CONFIG -= precompile_header
!win32 {
    OBJECTS_DIR = tmp
    MOC_DIR = tmp
}
DESTDIR = $$OUTPUT_DIR/plugin

!isEmpty(INSTALL_PREFIX) {
  target.path = $$INSTALL_PREFIX/$$INSTALL_LIBDIR/kst
  INSTALLS += target
}

INCLUDEPATH += \
	      tmp \
	      $$TOPLEVELDIR/src/libkst \
		  $$TOPLEVELDIR/src/libkstmath \


macx {
	LIBS += -F$$OUTPUT_DIR/lib
	qtAddLibrary(kst2lib)
 } else {
	LIBS += \
		-L$$OUTPUT_DIR/lib \
		-l$$kstlib(kst2lib)
}

LibExists(cfitsio) {
    win32 {
    
    } else {   
	CONFIG += link_pkgconfig
	PKGCONFIG += cfitsio
	INCLUDEPATH += $$pkginclude(cfitsio)
    }
}

LibExists(getdata) {
    win32{
	INCLUDEPATH += $$(GETDATADIR)/include
	LIBS += -L$$(GETDATADIR)/lib
    } else {
        macx {
            INCLUDEPATH += $$(GETDATADIR)/include
	    LIBS += -L$$(GETDATADIR)/lib
        } else {
	    CONFIG += link_pkgconfig
	    PKGCONFIG += getdata
	    INCLUDEPATH += $$pkginclude(getdata)
        }
    }
}

LibExists(netcdf) {
    win32 {	
	INCLUDEPATH += $$(NETCDFDIR)/include
	LIBS += -L$$(NETCDFDIR)/lib
    } else {
	CONFIG += link_pkgconfig
	PKGCONFIG += netcdf
	INCLUDEPATH += $$pkginclude(netcdf)
    }
}
