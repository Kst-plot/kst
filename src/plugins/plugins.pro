include($$PWD/../../config.pri)

TEMPLATE = subdirs
CONFIG += ordered

DEMOS += \
    sampleplugin 

SUBDIRS += \
    filters \
    dataobject

LibExists(gsl) {
  SUBDIRS += fits
}
