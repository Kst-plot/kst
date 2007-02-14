
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  kstfit_kneefrequency_unweighted.xml DESTINATION  )



#original Makefile.am contents follow:

#installdir=$(kde_moduledir)/kstplugins
#INCLUDES=-I$(srcdir)/../../../kst $(all_includes)
#
#install_LTLIBRARIES = kstfit_kneefrequency_unweighted.la
#
#kstfit_kneefrequency_unweighted_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#kstfit_kneefrequency_unweighted_la_SOURCES = kstfit_kneefrequency_unweighted.cpp
#
#METASOURCES=AUTO
#
#install_DATA=kstfit_kneefrequency_unweighted.xml
