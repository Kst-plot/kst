
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  butterworth_bandstop.xml DESTINATION  )



#original Makefile.am contents follow:

#installdir=$(kde_moduledir)/kstplugins
#INCLUDES=-I$(srcdir)/../../../kst $(all_includes)
#
#install_LTLIBRARIES = butterworth_bandstop.la
#
#butterworth_bandstop_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#butterworth_bandstop_la_LIBADD = $(GSL_LIBS)
#butterworth_bandstop_la_SOURCES = butterworth_bandstop.cpp
#
#METASOURCES=AUTO
#
#install_DATA=butterworth_bandstop.xml
