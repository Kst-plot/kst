
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#noinst_LTLIBRARIES = libfitstools.la
#
#libfitstools_la_LIBADD=$(FITSIO_LIBS)
#libfitstools_la_LDFLAGS=$(all_libraries) -module -avoid-version
#libfitstools_la_SOURCES = \
#	fitstools.cpp 
#
#METASOURCES=AUTO
