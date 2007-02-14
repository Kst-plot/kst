installdir=$(kde_moduledir)/kstplugins
INCLUDES=-I$(srcdir)/../../../kst $(all_includes)

install_LTLIBRARIES = kstfit_exponential_weighted.la

kstfit_exponential_weighted_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kstfit_exponential_weighted_la_LIBADD = $(GSL_LIBS)
kstfit_exponential_weighted_la_SOURCES = kstfit_exponential_weighted.cpp

METASOURCES=AUTO

install_DATA=kstfit_exponential_weighted.xml
