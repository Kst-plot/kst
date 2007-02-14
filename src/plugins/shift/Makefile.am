INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_shift.la

kstobject_shift_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_shift_la_SOURCES=shift.cpp

services_DATA=kstobject_shift.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
