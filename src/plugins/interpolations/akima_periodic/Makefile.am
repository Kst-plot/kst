INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_akima_periodic.la

kstobject_akima_periodic_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_akima_periodic_la_SOURCES=akima_periodic.cpp
kstobject_akima_periodic_la_LIBADD = $(GSL_LIBS)

services_DATA=kstobject_akima_periodic.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
