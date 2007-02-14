INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_polynomial.la

kstobject_polynomial_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_polynomial_la_SOURCES=polynomial.cpp
kstobject_polynomial_la_LIBADD = $(GSL_LIBS)

services_DATA=kstobject_polynomial.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO