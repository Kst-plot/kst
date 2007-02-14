if include_piolib
PIOLIB_SUBDIR=planck
endif

if include_fitsio_datasource
LIBFITSTOOLS_SUBDIR=libfitstools
LFIIO_SUBDIR=lfiio
WMAP_SUBDIR=wmap
FITSIMAGE_SUBDIR=fitsimage
HEALPIX_SUBDIR=healpix
endif

if include_cdf
CDF_SUBDIR=cdf
endif

if include_netcdf
NETCDF_SUBDIR=netcdf
endif

if include_nad
NAD_SUBDIR=nad
endif

SUBDIRS=ascii qimagesource dirfile frame indirect scuba2 $(PIOLIB_SUBDIR) $(CDF_SUBDIR) $(NETCDF_SUBDIR) $(LFIIO_SUBDIR) $(WMAP_SUBDIR) $(FITSIMAGE_SUBDIR) $(HEALPIX_SUBDIR) $(NAD_SUBDIR)

