INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(srcdir)/../../extdate $(all_includes)

kde_module_LTLIBRARIES=kstdata_indirect.la

kstdata_indirect_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_indirect_la_SOURCES=indirect.cpp

METASOURCES=AUTO

services_DATA=kstdata_indirect.desktop
servicesdir=$(kde_servicesdir)/kst

