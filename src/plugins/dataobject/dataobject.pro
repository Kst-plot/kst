include($$PWD/../../../config.pri)
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    bin \
    chop \
    crossspectrum \
    cumulativesum \
    differentiation \
    effectivebandwidth \
    genericfilter \
    linefit \
    periodogram \
    phase \
    shift \
    statistics \
    syncbin

  LibExists(gsl) {
     message(GSL configured.  Data Object plugins will be built.)
     SUBDIRS += convolution \
                correlation \
                interpolations \
                noiseaddition
     CONFIG += link_pkgconfig
     PKGCONFIG += gsl
  }
