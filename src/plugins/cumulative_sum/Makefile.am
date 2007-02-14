INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)

kde_module_LTLIBRARIES=kstobject_cumulative_sum.la

kstobject_cumulative_sum_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_cumulative_sum_la_SOURCES=cumulative_sum.cpp

services_DATA=kstobject_cumulative_sum.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
