

kst_init_plugin(src/datasources datasource)

if(MSVC)
	add_definitions(-DKST_USE_KST_ATOF)
endif()



kst_add_plugin(. ascii)
kst_add_plugin(. qimagesource)
kst_add_plugin(. sampledatasource)

if(getdata)
	include_directories(${GETDATA_INCLUDE_DIR})
	kst_add_plugin(. dirfilesource)
	kst_link(${GETDATA_LIBRARIES})
endif()

if(cfitsio)
	include_directories(${CFITSIO_INCLUDE_DIR})
	kst_add_plugin(. fitsimage)
	kst_link(${CFITSIO_LIBRARIES})
endif()

if(healpix)
	kst_add_plugin(. healpix)
endif()

if(lfioo)
	kst_add_plugin(. lfiio)
endif()

if(netcdf)
	include_directories(${NETCDF_INCLUDE_DIR})
	kst_add_plugin(. netcdf)
	kst_link(${NETCDF_LIBRARIES})
endif()
	
if(planckIDEF)
	kst_add_plugin(. planckIDEF)
endif()

if(dmc)
  include_directories(${DMC_INCLUDE_DIR})
  kst_add_plugin(. dmc)
  #kst_link(${DMC_LIBRARIES})
endif()



