INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)

kde_module_LTLIBRARIES = kstdata_planckIDEF.la

kstdata_planckIDEF_la_LDFLAGS = $(all_libraries) -module -avoid-version 
kstdata_planckIDEF_la_LIBADD = $(FITSIO_LIBS)
kstdata_planckIDEF_la_SOURCES = planckIDEF.cpp

METASOURCES=AUTO

services_DATA=kstdata_planckIDEF.desktop
servicesdir=$(kde_servicesdir)/kst

