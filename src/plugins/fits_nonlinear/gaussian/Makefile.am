installdir=$(kde_moduledir)/kstplugins
INCLUDES=-I$(srcdir)/../../../kst $(all_includes)

install_LTLIBRARIES = kstfit_gaussian.la

kstfit_gaussian_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kstfit_gaussian_la_LIBADD = $(GSL_LIBS)
kstfit_gaussian_la_SOURCES = kstfit_gaussian.cpp

METASOURCES=AUTO

install_DATA=kstfit_gaussian.xml
