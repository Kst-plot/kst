INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/widgets -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstapp  -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_linefit.la

kstobject_linefit_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_linefit_la_SOURCES=linefit.cpp

services_DATA=kstobject_linefit.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
