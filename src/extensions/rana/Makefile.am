INCLUDES=-I$(top_srcdir) -I$(top_srcdir)/kst $(KMDI_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES=kstextension_rana.la

kstextension_rana_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstextension_rana_la_LIBADD=../libkst/libkst.la $(LIB_KPARTS) $(GSL_LIBS)
kstextension_rana_la_SOURCES=rana.cpp noisetemperaturecalculation_i.cpp receiverinputoffset_i.cpp linearitygainisolation_i.cpp noisetemperaturecalculation.ui receiverinputoffset.ui linearitygainisolation.ui

METASOURCES=AUTO

services_DATA=kstextension_rana.desktop
servicesdir=$(kde_servicesdir)/kst

rcdir=$(kde_datadir)/kst
rc_DATA=kstextension_rana.rc

iconsdir = $(kde_icondir)/hicolor/22x22/actions/
icons_DATA = 
