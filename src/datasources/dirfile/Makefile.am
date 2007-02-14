INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)

kde_module_LTLIBRARIES=kstdata_dirfile.la

kstdata_dirfile_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_dirfile_la_SOURCES=dirfile.cpp getdata.c

METASOURCES=AUTO

services_DATA=kstdata_dirfile.desktop
servicesdir=$(kde_servicesdir)/kst

