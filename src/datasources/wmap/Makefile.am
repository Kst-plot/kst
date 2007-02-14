INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)

kde_module_LTLIBRARIES = kstdata_wmap.la

kstdata_wmap_la_LDFLAGS = $(all_libraries) -module -avoid-version 
kstdata_wmap_la_LIBADD = $(FITSIO_LIBS)
kstdata_wmap_la_SOURCES = wmap.cpp

METASOURCES=AUTO

services_DATA=kstdata_wmap.desktop
servicesdir=$(kde_servicesdir)/kst

