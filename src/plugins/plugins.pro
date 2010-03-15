include($$PWD/../../config.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    sampleplugin \
    filters \
    dataobject

LibExists(gsl) {
  SUBDIRS += fits
}
