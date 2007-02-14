INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_discretizing_filters.la

kstobject_discretizing_filters_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_discretizing_filters_la_SOURCES=genericfilter.cpp

services_DATA=kstobject_discretizing_filters.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
