INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I../../libkst -I$(top_srcdir)/kst/src/libkstapp -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES=kstextension_elog.la

kstextension_elog_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstextension_elog_la_LIBADD=../../libkst/libkst.la $(LIB_KPARTS)
kstextension_elog_la_SOURCES=elog.cpp elogthread.cpp elogbasicthreadsubmit.cpp elogeventthreadsubmit.cpp elogthreadsubmit.cpp elogthreadattrs.cpp elogentry_i.cpp elogevententry_i.cpp elogconfiguration_i.cpp elogentry.ui elogevententry.ui elogconfiguration.ui

METASOURCES=AUTO

services_DATA=kstextension_elog.desktop
servicesdir=$(kde_servicesdir)/kst

rcdir=$(kde_datadir)/kst
rc_DATA=kstextension_elog.rc

iconsdir = $(kde_icondir)/hicolor/22x22/actions/
icons_DATA = addelogentry.png launchelogbrowser.png

