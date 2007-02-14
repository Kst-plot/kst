
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  kstfit_lorentzian.xml DESTINATION  )



#original Makefile.am contents follow:

#installdir=$(kde_moduledir)/kstplugins
#INCLUDES=-I$(srcdir)/../../../kst $(all_includes)
#
#install_LTLIBRARIES = kstfit_lorentzian.la
#
#kstfit_lorentzian_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#kstfit_lorentzian_la_LIBADD = $(GSL_LIBS)
#kstfit_lorentzian_la_SOURCES = kstfit_lorentzian.cpp
#
#METASOURCES=AUTO
#
#install_DATA=kstfit_lorentzian.xml
