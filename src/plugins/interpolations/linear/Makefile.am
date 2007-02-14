INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_linear.la

kstobject_linear_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_linear_la_SOURCES=linear.cpp
kstobject_linear_la_LIBADD = $(GSL_LIBS)

services_DATA=kstobject_linear.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO