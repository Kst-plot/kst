include($$PWD/../../config.pri)
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    ascii \
    qimagesource \
    sampledatasource


 contains(HAVE_CFITSIO, 1) {
     message(CFITSIO configured.  Plugins will be built.)
     SUBDIRS += fitsimage
 }

contains(HAVE_DIRFILE, 1) {
     message(DIRFILE configured.  Plugins will be built.)
  !win32:SUBDIRS += dirfilesource
}
