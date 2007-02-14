INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate -I$(top_srcdir) -I$(top_srcdir)/kst/src/libkstmath -I$(top_srcdir)/kst/src/libkstapp $(KMDI_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES=kstextension_pulse.la

kstextension_pulse_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstextension_pulse_la_LIBADD=../../libkst/libkst.la $(LIB_KPARTS)
kstextension_pulse_la_SOURCES=pulse.cpp kstpulsegenerator.ui kstgvector.cpp

METASOURCES=AUTO

services_DATA=kstextension_pulse.desktop
servicesdir=$(kde_servicesdir)/kst

rcdir=$(kde_datadir)/kst
rc_DATA=kstextension_pulse.rc

iconsdir = $(kde_icondir)/hicolor/22x22/actions/
icons_DATA = 
