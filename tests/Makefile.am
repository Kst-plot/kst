
if include_fitsio_datasource
HEALPIX_DIR=healpix
endif

SUBDIRS=. kstXbench $(HEALPIX_DIR)

check_PROGRAMS=testeqparser testhistogram testscalars testlabelparser \
		testvector testrvector \
		testmatrix testamatrix testcsd testpsd testsmatrix

METASOURCES=AUTO

INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I$(top_srcdir)/kst/src/extdate $(all_includes)

AM_LDFLAGS = $(QT_LDFLAGS) $(KDE_LDFLAGS) $(X_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KDECORE) ../src/libkstmath/libkstmath.la ../src/libkst/libkst.la
testeqparser_SOURCES=testeqparser.cpp
testhistogram_SOURCES=testhistogram.cpp
testscalars_SOURCES=testscalars.cpp
testlabelparser_SOURCES=testlabelparser.cpp
testvector_SOURCES=testvector.cpp
testrvector_SOURCES=testrvector.cpp
testmatrix_SOURCES=testmatrix.cpp
testamatrix_SOURCES=testamatrix.cpp
testcsd_SOURCES=testcsd.cpp
testpsd_SOURCES=testpsd.cpp
testsmatrix_SOURCES=testsmatrix.cpp
