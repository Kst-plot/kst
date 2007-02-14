INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_phase.la

kstobject_phase_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_phase_la_SOURCES=phase.cpp

services_DATA=kstobject_phase.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
