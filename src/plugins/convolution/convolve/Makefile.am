INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_convolve.la

kstobject_convolve_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_convolve_la_SOURCES=convolve.cpp
kstobject_convolve_la_LIBADD = $(GSL_LIBS)

services_DATA=kstobject_convolve.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
