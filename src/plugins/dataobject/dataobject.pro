include($$PWD/../../../config.pri)

TEMPLATE = subdirs
CONFIG += ordered

DEMOS += \
    linefit \
    genericfilter 

SUBDIRS += \
    bin \
    chop \
    crossspectrum \
    effectivebandwidth \
    periodogram \
    phase \
    shift \
    statistics \
    syncbin

LibExists(gsl) {
     SUBDIRS += convolution \
                correlation \
                interpolations \
                noiseaddition
}
