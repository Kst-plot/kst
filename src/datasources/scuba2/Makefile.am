INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)

kde_module_LTLIBRARIES=kstdata_scuba.la

kstdata_scuba_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_scuba_la_SOURCES=scuba.cpp scubaconfig.ui

METASOURCES=AUTO

services_DATA=kstdata_scuba.desktop
servicesdir=$(kde_servicesdir)/kst

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kst.pot
