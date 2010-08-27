include($$PWD/../../config.pri)
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    ascii \
    qimagesource \
    sampledatasource

LibExists(cfitsio) {
    message(CFITSIO configured.  Plugins will be built.)
    SUBDIRS += fitsimage 
}

LibExists(getdata) {
    message(DIRFILE configured.  Plugins will be built.)
    SUBDIRS += dirfilesource
}

LibExists(netcdf) {
    message(netCDF configured.  Plugins will be built.)
    SUBDIRS += netcdf
}

