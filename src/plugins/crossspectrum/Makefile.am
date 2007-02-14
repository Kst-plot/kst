INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/widgets -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstapp -I$(top_srcdir)/kst/src/libkstmath -I$(top_srcdir)/kst/src/extdate -I$(top_builddir)/kst -I$(top_builddir)/kst/src/widgets -I$(top_builddir)/kst/src/libkst -I$(top_builddir)/kst/src/libkstapp -I$(top_builddir)/kst/src/libkstmath -I$(top_builddir)/kst/src/extdate $(all_includes)

kde_module_LTLIBRARIES=kstobject_crossspectrum.la

kstobject_crossspectrum_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstobject_crossspectrum_la_SOURCES=crosspowerspectrum.cpp crossspectrumdialog_i.cpp crossspectrumdialogwidget.ui fftsg_h.c

services_DATA=kstobject_crossspectrum.desktop
servicesdir=$(kde_servicesdir)/kst

METASOURCES=AUTO
