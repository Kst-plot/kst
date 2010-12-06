

kst_init_plugin(src/datasources datasource)

if(MSVC)
	add_definitions(-DKST_USE_KST_ATOF)
endif()

kst_add_plugin(. ascii)
kst_add_plugin(. qimagesource)
kst_add_plugin(. sampledatasource)

if(getdata)
	message(STATUS "Building 'dirfile' datasource plugin")
	include_directories(${GETDATA_INCLUDE_DIR})
	kst_add_plugin(. dirfilesource)
	kst_link(${GETDATA_LIBRARIES})
endif()

if(fitsimage)
	kst_add_plugin(. fitsimage)
endif()

if(healpix)
	kst_add_plugin(. healpix)
endif()

if(lfioo)
	kst_add_plugin(. lfiio)
endif()

if(netcdf)
	message(STATUS "Building 'netcdf' datasource plugin")
	include_directories(${NETCDF_INCLUDE_DIR})
	kst_add_plugin(. netcdf)
	kst_link(${NETCDF_LIBRARIES})
endif()
	
if(planckIDEF)
	kst_add_plugin(. planckIDEF)
endif()
