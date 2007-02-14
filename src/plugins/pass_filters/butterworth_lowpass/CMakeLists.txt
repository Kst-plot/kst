
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  butterworth_lowpass.xml DESTINATION  )



#original Makefile.am contents follow:

#installdir=$(kde_moduledir)/kstplugins
#INCLUDES=-I$(srcdir)/../../../kst $(all_includes)
#
#install_LTLIBRARIES = butterworth_lowpass.la
#
#butterworth_lowpass_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#butterworth_lowpass_la_LIBADD = $(GSL_LIBS)
#butterworth_lowpass_la_SOURCES = butterworth_lowpass.cpp
#
#METASOURCES=AUTO
#
#install_DATA=butterworth_lowpass.xml
