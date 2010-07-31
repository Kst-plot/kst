include($$PWD/../../../config.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    despike \
    cumulativesum \
    differentiation

LibExists(gsl) {
     message(GSL configured.  Filter plugins will be built.)
     SUBDIRS += butterworth_bandpass \
                butterworth_bandstop \
                butterworth_highpass \
                butterworth_lowpass
 }
