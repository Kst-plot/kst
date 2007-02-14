installdir=$(kde_moduledir)/kstplugins
INCLUDES=-I$(srcdir)/../../../kst $(all_includes)

install_LTLIBRARIES = kstfit_sinusoid_weighted.la

kstfit_sinusoid_weighted_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kstfit_sinusoid_weighted_la_LIBADD = $(GSL_LIBS)
kstfit_sinusoid_weighted_la_SOURCES = kstfit_sinusoid_weighted.cpp

METASOURCES=AUTO

install_DATA=kstfit_sinusoid_weighted.xml
