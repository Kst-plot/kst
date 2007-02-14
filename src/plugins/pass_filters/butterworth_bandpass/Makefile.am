installdir=$(kde_moduledir)/kstplugins
INCLUDES=-I$(srcdir)/../../../kst $(all_includes)

install_LTLIBRARIES = butterworth_bandpass.la

butterworth_bandpass_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
butterworth_bandpass_la_LIBADD = $(GSL_LIBS)
butterworth_bandpass_la_SOURCES = butterworth_bandpass.cpp

METASOURCES=AUTO

install_DATA=butterworth_bandpass.xml
