INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_statistics.la

kstobject_statistics_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_statistics_la_SOURCES=statistics.cpp

services_DATA=kstobject_statistics.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
