INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(NETCDFINCLUDES) $(all_includes)

kde_module_LTLIBRARIES=kstdata_netcdf.la

kstdata_netcdf_la_LIBADD=$(NETCDFLIBS)
kstdata_netcdf_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_netcdf_la_SOURCES=netcdf.cpp

METASOURCES=AUTO

services_DATA=kstdata_netcdf.desktop
servicesdir=$(kde_servicesdir)/kst

