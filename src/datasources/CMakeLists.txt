

# kst_init_plugin(src/datasources datasource)

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
kst_add_plugin(. sourcelist)

if(TARGET Getdata::getdata)
	include_directories(${GETDATA_INCLUDE_DIR})
  kst_add_plugin(. dirfilesource LINK_LIBRARIES ${GETDATA_LIBRARIES})

endif()

if(TARGET CFITSIO::CFITSIO)
	include_directories(${CFITSIO_INCLUDE_DIR})
	kst_add_plugin(. fitsimage LINK_LIBRARIES ${CFITSIO_LIBRARIES})
  #kst_add_plugin(. fitstable)
	#kst_add_plugin(. healpix)
endif()

if(TARGET Netcdf::Netcdf)
	include_directories(${NETCDF_INCLUDE_DIR})
	kst_add_plugin(. netcdf LINK_LIBRARIES  ${NETCDF_LIBRARIES})
endif()
	
if(TARGET Matio::Matio)
	include_directories(${MATIO_INCLUDE_DIR})
	kst_add_plugin(. matlab LINK_LIBRARIES ${MATIO_LIBRARIES}) # Directory named after the file format, not the lib
endif()

if(TARGET TIFF::TIFF)
	kst_add_plugin(. tiff16source LINK_LIBRARIES TIFF::TIFF) # Directory named after the file format, not the lib
endif()

if(TARGET HDF5::HDF5)
  include_directories(${HDF5_INCLUDE_DIR})
	kst_add_plugin(. hdf5 LINK_LIBRARIES ${HDF5_LIBRARIES})
endif()
