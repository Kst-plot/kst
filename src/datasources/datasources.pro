TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    ascii \
    qimagesource \
    sampledatasource


!win32:SUBDIRS += dirfile

#     cdf \
#     fitsimage \
#     frame \
#     healpix \
#     indirect \
#     lfiio \
#     libfitstools \
#     nad \
#     naddirect \
#     netcdf \
#     planck \
#     planckIDEF \
#     qimagesource \
#     scuba2 \
#     template \
#     wmap
