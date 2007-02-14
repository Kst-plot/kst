bin_PROGRAMS = d2d

# set the include path for X, qt and KDE
INCLUDES=-I$(top_srcdir)/kst/src/extdate -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I../libkst -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)

d2d_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined
d2d_LDADD = ../libkstmath/libkstmath.la
d2d_SOURCES = d2d.cpp


