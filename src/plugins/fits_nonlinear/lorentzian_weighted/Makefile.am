installdir=$(kde_moduledir)/kstplugins
INCLUDES=-I$(srcdir)/../../../kst $(all_includes)

install_LTLIBRARIES = kstfit_lorentzian_weighted.la

kstfit_lorentzian_weighted_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kstfit_lorentzian_weighted_la_LIBADD = $(GSL_LIBS)
kstfit_lorentzian_weighted_la_SOURCES = kstfit_lorentzian_weighted.cpp

METASOURCES=AUTO

install_DATA=kstfit_lorentzian_weighted.xml
