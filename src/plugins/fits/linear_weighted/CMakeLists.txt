
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  kstfit_linear_weighted.xml DESTINATION  )



#original Makefile.am contents follow:

#installdir=$(kde_moduledir)/kstplugins
#INCLUDES=-I$(srcdir)/../../../kst $(all_includes)
#
#install_LTLIBRARIES = kstfit_linear_weighted.la
#
#kstfit_linear_weighted_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#kstfit_linear_weighted_la_LIBADD = $(GSL_LIBS)
#kstfit_linear_weighted_la_SOURCES = kstfit_linear_weighted.cpp
#
#METASOURCES=AUTO
#
#install_DATA=kstfit_linear_weighted.xml
