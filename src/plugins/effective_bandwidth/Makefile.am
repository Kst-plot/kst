INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_effbandwidth.la

kstobject_effbandwidth_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_effbandwidth_la_SOURCES=effective_bandwidth.cpp

services_DATA=kstobject_effbandwidth.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
