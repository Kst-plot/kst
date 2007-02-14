installdir=$(kde_moduledir)/kstplugins
INCLUDES=-I$(srcdir)/../../../kst $(all_includes)

install_LTLIBRARIES = kstfit_exponential.la

kstfit_exponential_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kstfit_exponential_la_LIBADD = $(GSL_LIBS)
kstfit_exponential_la_SOURCES = kstfit_exponential.cpp

METASOURCES=AUTO

install_DATA=kstfit_exponential.xml
