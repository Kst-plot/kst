
add_subdirectory( butterworth_lowpass )
add_subdirectory( butterworth_highpass )
add_subdirectory( butterworth_bandpass )
add_subdirectory( butterworth_bandstop )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############




#original Makefile.am contents follow:

#SUBDIRS=butterworth_lowpass butterworth_highpass butterworth_bandpass butterworth_bandstop
#
