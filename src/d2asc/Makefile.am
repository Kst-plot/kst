bin_PROGRAMS = d2asc

# set the include path for X, qt and KDE
INCLUDES=-I$(top_srcdir)/kst/src/extdate -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I../libkst -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)

d2asc_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined
d2asc_LDADD = ../libkstmath/libkstmath.la
d2asc_SOURCES = d2asc.cpp


