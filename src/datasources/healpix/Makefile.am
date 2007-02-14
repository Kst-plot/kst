INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)

kde_module_LTLIBRARIES=kstdata_healpix.la

kstdata_healpix_la_LIBADD=$(FITSIO_LIBS)
kstdata_healpix_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_healpix_la_SOURCES=healpix.cpp healpix_tools_pix.cpp healpix_tools_proj.cpp healpix_tools_fits.cpp healpixconfig.ui

METASOURCES=AUTO

services_DATA=kstdata_healpix.desktop
servicesdir=$(kde_servicesdir)/kst

