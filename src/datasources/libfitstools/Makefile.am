INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)

noinst_LTLIBRARIES = libfitstools.la

libfitstools_la_LIBADD=$(FITSIO_LIBS)
libfitstools_la_LDFLAGS=$(all_libraries) -module -avoid-version
libfitstools_la_SOURCES = \
	fitstools.cpp 

METASOURCES=AUTO
