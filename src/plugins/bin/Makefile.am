INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_bin.la

kstobject_bin_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_bin_la_SOURCES=bin.cpp

services_DATA=kstobject_bin.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
