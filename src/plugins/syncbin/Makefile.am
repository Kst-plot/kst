INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_syncbin.la

kstobject_syncbin_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_syncbin_la_SOURCES=syncbin.cpp

services_DATA=kstobject_syncbin.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
