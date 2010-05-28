include($$PWD/../../config.pri)
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    ascii \
    qimagesource \
    sampledatasource

LibExists(cfitsio) {
    message(CFITSIO configured.  Plugins will be built.)
#    SUBDIRS += fitsimage lfiio healpix planckIDEF
#    CONFIG += link_pkgconfig
#    PKGCONFIG += cfitsio
}

LibExists(getdata) {
    message(DIRFILE configured.  Plugins will be built.)
    SUBDIRS += dirfilesource
    CONFIG += link_pkgconfig
    PKGCONFIG += getdata
}

LibExists(netcdf) {
    message(netCDF configured.  Plugins will be built.)
    SUBDIRS += netcdf
    !win32:CONFIG += link_pkgconfig
    !win32:PKGCONFIG += netcdf
}

