

kst_init_plugin(src/datasources datasource)

if(MSVC)
	add_definitions(-DKST_USE_KST_ATOF)
endif()

set(msg "> ")
message(STATUS "Building datasource plugins---------------------------------")
kst_add_plugin(. ascii            ${msg})
kst_add_plugin(. qimagesource     ${msg})
kst_add_plugin(. sampledatasource ${msg})

if(getdata)
	include_directories(${GETDATA_INCLUDE_DIR})
	kst_add_plugin(. dirfilesource ${msg})
	kst_link(${GETDATA_LIBRARIES})
endif()

if(cfitsio)
	include_directories(${CFITSIO_INCLUDE_DIR})
	kst_add_plugin(. fitsimage ${msg})
	kst_link(${CFITSIO_LIBRARIES})
endif()

if(healpix)
	kst_add_plugin(. healpix ${msg})
endif()

if(lfioo)
	kst_add_plugin(. lfiio ${msg})
endif()

if(netcdf)
	include_directories(${NETCDF_INCLUDE_DIR})
	kst_add_plugin(. netcdf ${msg})
	kst_link(${NETCDF_LIBRARIES})
endif()
	
if(planckIDEF)
	kst_add_plugin(. planckIDEF ${msg})
endif()


message(STATUS "------------------------------------------------------------")
message(STATUS)


