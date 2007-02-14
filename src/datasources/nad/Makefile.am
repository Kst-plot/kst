SUBDIRS=nadslave

INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(NADINCLUDES) $(all_includes)

kde_module_LTLIBRARIES=kstdata_nad.la

kstdata_nad_la_LIBADD=$(NADLIBS)
kstdata_nad_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_nad_la_SOURCES=nadsource.cpp

METASOURCES=AUTO

services_DATA=kstdata_nad.desktop
servicesdir=$(kde_servicesdir)/kst

