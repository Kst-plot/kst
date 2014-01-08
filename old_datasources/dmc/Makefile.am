INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(srcdir)/../../extdate $(DMCINCLUDES) $(TFINCLUDES) $(all_includes)

kde_module_LTLIBRARIES=kstdata_dmc.la

kstdata_dmc_la_LIBADD=$(DMCLIBS) $(TRANSFUNCS)
kstdata_dmc_la_LDFLAGS=$(all_libraries) -module -avoid-version
kstdata_dmc_la_SOURCES=dmc.cpp dmcdata.cpp dmcobj.cpp

METASOURCES=AUTO

services_DATA=kstdata_dmc.desktop
servicesdir=$(kde_servicesdir)/kst

