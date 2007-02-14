
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  butterworth_highpass.xml DESTINATION  )



#original Makefile.am contents follow:

#installdir=$(kde_moduledir)/kstplugins
#INCLUDES=-I$(srcdir)/../../../kst $(all_includes)
#
#install_LTLIBRARIES = butterworth_highpass.la
#
#butterworth_highpass_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#butterworth_highpass_la_LIBADD = $(GSL_LIBS)
#butterworth_highpass_la_SOURCES = butterworth_highpass.cpp
#
#METASOURCES=AUTO
#
#install_DATA=butterworth_highpass.xml
