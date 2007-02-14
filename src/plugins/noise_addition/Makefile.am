INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_noise_addition.la

kstobject_noise_addition_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_noise_addition_la_SOURCES=noise_addition.cpp
kstobject_noise_addition_la_LIBADD = $(GSL_LIBS)

services_DATA=kstobject_noise_addition.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
