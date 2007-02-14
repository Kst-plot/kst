INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_autocorrelate.la

kstobject_autocorrelate_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_autocorrelate_la_SOURCES=autocorrelate.cpp
kstobject_autocorrelate_la_LIBADD = $(GSL_LIBS)

services_DATA=kstobject_autocorrelate.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
