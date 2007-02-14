INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)

kde_module_LTLIBRARIES=kstdata_ascii.la

kstdata_ascii_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_ascii_la_SOURCES=ascii.cpp asciiconfig.ui

METASOURCES=AUTO

services_DATA=kstdata_ascii.desktop
servicesdir=$(kde_servicesdir)/kst

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kst.pot
