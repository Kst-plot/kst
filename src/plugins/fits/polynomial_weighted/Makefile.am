installdir=$(kde_moduledir)/kstplugins
INCLUDES=-I$(srcdir)/../../../kst $(all_includes)

install_LTLIBRARIES = kstfit_polynomial_weighted.la

kstfit_polynomial_weighted_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kstfit_polynomial_weighted_la_LIBADD = $(GSL_LIBS)
kstfit_polynomial_weighted_la_SOURCES = kstfit_polynomial_weighted.cpp

METASOURCES=AUTO

install_DATA=kstfit_polynomial_weighted.xml
