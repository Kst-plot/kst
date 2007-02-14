
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  kstfit_gradient_weighted.xml DESTINATION  )



#original Makefile.am contents follow:

#installdir=$(kde_moduledir)/kstplugins
#INCLUDES=-I$(srcdir)/../../../kst $(all_includes)
#
#install_LTLIBRARIES = kstfit_gradient_weighted.la
#
#kstfit_gradient_weighted_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#kstfit_gradient_weighted_la_LIBADD = $(GSL_LIBS)
#kstfit_gradient_weighted_la_SOURCES = kstfit_gradient_weighted.cpp
#
#METASOURCES=AUTO
#
#install_DATA=kstfit_gradient_weighted.xml
