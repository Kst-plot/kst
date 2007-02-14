
check_PROGRAMS=testhealpix

METASOURCES=AUTO

INCLUDES=-I$(top_srcdir)/kst/kst/datasources/healpix -I$(top_srcdir)/kst/kst -I$(top_srcdir)/kst/tests $(all_includes)

AM_LDFLAGS = $(QT_LDFLAGS) $(KDE_LDFLAGS) $(X_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KDECORE) ../../kst/libkstmath.la ../../kst/libkst.la ../../kst/libkstnonguiapp.la ../../kst/datasources/healpix/kstdata_healpix.la
testhealpix_SOURCES=testhealpix.cpp

