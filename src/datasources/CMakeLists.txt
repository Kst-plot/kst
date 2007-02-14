
add_subdirectory( ascii )
add_subdirectory( qimagesource )
add_subdirectory( dirfile )
add_subdirectory( frame )
add_subdirectory( indirect )
add_subdirectory( scuba2 )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(PIOLIB_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(CDF_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(NETCDF_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(LFIIO_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(WMAP_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(FITSIMAGE_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(HEALPIX_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(NAD_SUBDIR)")
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############




#original Makefile.am contents follow:

#if include_piolib
#PIOLIB_SUBDIR=planck
#endif
#
#if include_fitsio_datasource
#LIBFITSTOOLS_SUBDIR=libfitstools
#LFIIO_SUBDIR=lfiio
#WMAP_SUBDIR=wmap
#FITSIMAGE_SUBDIR=fitsimage
#HEALPIX_SUBDIR=healpix
#endif
#
#if include_cdf
#CDF_SUBDIR=cdf
#endif
#
#if include_netcdf
#NETCDF_SUBDIR=netcdf
#endif
#
#if include_nad
#NAD_SUBDIR=nad
#endif
#
#SUBDIRS=ascii qimagesource dirfile frame indirect scuba2 $(PIOLIB_SUBDIR) $(CDF_SUBDIR) $(NETCDF_SUBDIR) $(LFIIO_SUBDIR) $(WMAP_SUBDIR) $(FITSIMAGE_SUBDIR) $(HEALPIX_SUBDIR) $(NAD_SUBDIR)
#
