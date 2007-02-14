INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_deconvolve.la

kstobject_deconvolve_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_deconvolve_la_SOURCES=deconvolve.cpp
kstobject_deconvolve_la_LIBADD = $(GSL_LIBS)

services_DATA=kstobject_deconvolve.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
