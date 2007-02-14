installdir=$(kde_moduledir)/kstplugins
INCLUDES=-I$(srcdir)/../../../kst $(all_includes)

install_LTLIBRARIES = despike.la

despike_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
despike_la_SOURCES = despike.cpp

METASOURCES=AUTO

install_DATA=despike.xml