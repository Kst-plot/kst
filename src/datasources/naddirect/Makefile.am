INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)

kde_module_LTLIBRARIES=kstdata_naddirect.la

kstdata_naddirect_la_LIBADD=$(LIB_KIO) -lcurl
kstdata_naddirect_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_naddirect_la_SOURCES=naddirect.cpp nadconnection.cpp

METASOURCES=AUTO

services_DATA=kstdata_naddirect.desktop
servicesdir=$(kde_servicesdir)/kst

