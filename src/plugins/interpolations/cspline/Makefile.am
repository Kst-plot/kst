INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_cspline.la

kstobject_cspline_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_cspline_la_SOURCES=cspline.cpp
kstobject_cspline_la_LIBADD = $(GSL_LIBS)

services_DATA=kstobject_cspline.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO