INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_periodogram.la

kstobject_periodogram_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_periodogram_la_SOURCES=periodogram.cpp

services_DATA=kstobject_periodogram.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
