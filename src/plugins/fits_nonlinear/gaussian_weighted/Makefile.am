installdir=$(kde_moduledir)/kstplugins
INCLUDES=-I$(srcdir)/../../../kst $(all_includes)

install_LTLIBRARIES = kstfit_gaussian_weighted.la

kstfit_gaussian_weighted_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kstfit_gaussian_weighted_la_LIBADD = $(GSL_LIBS)
kstfit_gaussian_weighted_la_SOURCES = kstfit_gaussian_weighted.cpp

METASOURCES=AUTO

install_DATA=kstfit_gaussian_weighted.xml
