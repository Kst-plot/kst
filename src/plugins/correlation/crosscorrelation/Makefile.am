INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_crosscorrelate.la

kstobject_crosscorrelate_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_crosscorrelate_la_SOURCES=crosscorrelate.cpp
kstobject_crosscorrelate_la_LIBADD = $(GSL_LIBS)

services_DATA=kstobject_crosscorrelate.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
