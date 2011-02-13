
include(FindPkgConfig)

pkg_check_modules(NETCDF QUIET netcdf)

if(NETCDF_INCLUDEDIR AND NETCDF_LIBRARIES)
	FIND_LIBRARY(NETCDF_LIBRARY_CPP netcdf_c++ 
		HINTS ${NETCDF_LIBRARY_DIRS})	
	set(NETCDF_LIBRARY_C -L${NETCDF_LIBRARY_DIRS} ${NETCDF_LIBRARIES})
else()
	set(NETCDF_INCLUDEDIR NETCDF_INCLUDEDIR-NOTFOUND CACHE STRING "" FORCE)
	FIND_PATH(NETCDF_INCLUDEDIR netcdf.h
		HINTS
		ENV NETCDF_DIR
		PATH_SUFFIXES include
		PATHS 
		${kst_3rdparty_dir}
		~/Library/Frameworks
		/Library/Frameworks
		)
		
	macro(find_netcdf_lib var libname)
		FIND_LIBRARY(${var} ${libname} 
			HINTS
			ENV NETCDF_DIR
			PATH_SUFFIXES lib
			PATHS ${kst_3rdparty_dir})
	endmacro()
	
	find_netcdf_lib(netcdf_c         netcdf)
	find_netcdf_lib(netcdf_c_debug   netcdfd)
	find_netcdf_lib(netcdf_cpp       netcdf_c++)
	find_netcdf_lib(netcdf_cpp_debug netcdf_c++d)
	
	if(netcdf_c AND netcdf_c_debug)
		set(NETCDF_LIBRARY_C optimized ${netcdf_c} debug ${netcdf_c_debug})
	endif()
	if(netcdf_cpp AND netcdf_cpp_debug)
	   set(NETCDF_LIBRARY_CPP optimized ${netcdf_cpp} debug ${netcdf_cpp_debug})
	endif()
	
	if(NOT MSVC)
		# only msvc needs debug and release
		set(NETCDF_LIBRARY_C   ${netcdf_c})
		set(NETCDF_LIBRARY_CPP ${netcdf_cpp})
	endif()
endif()

#message(STATUS "NETCDF: ${NETCDF_INCLUDEDIR}")
#message(STATUS "NETCDF: ${NETCDF_LIBRARY_C}")
#message(STATUS "NETCDF: ${NETCDF_LIBRARY_CPP}")
IF(NETCDF_INCLUDEDIR AND NETCDF_LIBRARY_C AND NETCDF_LIBRARY_CPP)
    SET(NETCDF_LIBRARIES ${NETCDF_LIBRARY_CPP} ${NETCDF_LIBRARY_C})
	SET(NETCDF_INCLUDE_DIR ${NETCDF_INCLUDEDIR})
	SET(netcdf 1)
	message(STATUS "Found NetCDF:")
	message(STATUS "     includes : ${NETCDF_INCLUDE_DIR}")
	message(STATUS "     libraries: ${NETCDF_LIBRARIES}")
ELSE()
	MESSAGE(STATUS "Not found: NetCDF")
ENDIF()



