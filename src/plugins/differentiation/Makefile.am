INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_differentiation.la

kstobject_differentiation_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_differentiation_la_SOURCES=differentiation.cpp

services_DATA=kstobject_differentiation.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
