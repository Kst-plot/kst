

kst_init_plugin(src/datasources datasource)

if(MSVC OR MINGW)
	add_definitions(-DKST_USE_KST_ATOF)
endif()

if(CMAKE_COMPILER_IS_GNUCC AND NOT GCC_VERSION VERSION_GREATER 4.2)
    add_definitions(-DKST_NO_THREAD_LOCAL)
endif()


if(UNIX)
  # speedup on Linux x2.7
  add_definitions(-DKST_USE_KST_ATOF)
endif()

kst_add_plugin(. ascii)
kst_add_plugin(. qimagesource)
#kst_add_plugin(. sampledatasource)
kst_add_plugin(. sourcelist)
kst_add_plugin(. bis)
kst_add_plugin(. its)

if(getdata)
	include_directories(${GETDATA_INCLUDE_DIR})
	kst_add_plugin(. dirfilesource LINK_LIBRARIES ${GETDATA_LIBRARIES})
endif()

if(cfitsio)
	include_directories(${CFITSIO_INCLUDE_DIR})
	kst_add_plugin(. fitsimage LINK_LIBRARIES ${CFITSIO_LIBRARIES})
  #kst_add_plugin(. fitstable)
	#kst_add_plugin(. healpix)
endif()

#if(healpix)
#	kst_add_plugin(. healpix)
#endif()

#if(lfioo)
#	kst_add_plugin(. lfiio)
#endif()

if(netcdf)
	include_directories(${NETCDF_INCLUDE_DIR})
	kst_add_plugin(. netcdf LINK_LIBRARIES ${NETCDF_LIBRARIES})
endif()
	
#if(planckIDEF)
#	kst_add_plugin(. planckIDEF)
#endif()

#if(dmc)
#  include_directories(${DMC_INCLUDE_DIR})
#  kst_add_plugin(. dmc)
#  #kst_link(${DMC_LIBRARIES})
#endif()

if(matio)
	include_directories(${MATIO_INCLUDE_DIR})
	kst_add_plugin(. matlab LINK_LIBRARIES ${MATIO_LIBRARIES}) # Directory named after the file format, not the lib
endif()

if(TARGET TIFF::TIFF)
	kst_add_plugin(. tiff16source LINK_LIBRARIES TIFF::TIFF) # Directory named after the file format, not the lib
endif()

if(hdf5)
        include_directories(${HDF5_INCLUDEDIR})
	kst_add_plugin(. hdf5 LINK_LIBRARIES ${HDF5_LIBRARIES})
endif()
