

kst_init_plugin(src/datasources)

kst_add_plugin(. ascii)
kst_add_plugin(. qimagesource)
kst_add_plugin(. sampledatasource)

if(getdata)
	kst_add_plugin(. dirfilesource)
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
	kst_add_plugin(. netcdf)
endif()
	
if(planckIDEF)
	kst_add_plugin(. planckIDEF)
endif()
