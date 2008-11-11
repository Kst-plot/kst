include($$PWD/../../../config.pri)
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    despike 

 contains(HAVE_GSL, 1) {
     message(GSL configured.  Filter plugins will be built.)
     SUBDIRS += butterworth_bandpass \
                butterworth_bandstop \
                butterworth_highpass \
                butterworth_lowpass
 }
