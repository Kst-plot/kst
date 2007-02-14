INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)

kde_module_LTLIBRARIES=kstdata_frame.la

kstdata_frame_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_frame_la_SOURCES=frame.cpp readdata.c creaddata.c

METASOURCES=AUTO

services_DATA=kstdata_frame.desktop
servicesdir=$(kde_servicesdir)/kst

