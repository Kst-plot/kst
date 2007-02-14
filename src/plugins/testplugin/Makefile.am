INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_testplugin.la

kstobject_testplugin_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_testplugin_la_SOURCES=testplugin.cpp

services_DATA=kstobject_testplugin.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
