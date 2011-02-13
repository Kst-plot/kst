
include(FindPkgConfig)

pkg_check_modules(PKGGETDATA QUIET getdata>=0.6.0)
#message(STATUS "GD inc: ${PKGGETDATA_INCLUDEDIR}")
#message(STATUS "GD libs: ${PKGGETDATA_LIBRARIES}")


# Apple: install getdata with --prefix /opt/local

# FIXME: GETDATA_INCLUDEDIR AND GETDATA_LIBRARIES are set by pkg_check_modules, but
# GETDATA_LIBRARY_C and GETDATA_LIBRARY_CPP are not.
# Ubuntu: maybe /usr/local/lib/pkgconfig/getdata.pc is not correct
#if(NOT PKGGETDATA_LIBRARIES)
	set(PKGGETDATA_LIBRARIES getdata++ getdata)
    if (UNIX)
		SET(PKGGETDATA_LIBRARIES ${PKGGETDATA_LIBRARIES} m)
	endif()
#endif()


set(GETDATA_INCLUDEDIR GETDATA_INCLUDEDIR-NOTFOUND CACHE STRING "" FORCE)
FIND_PATH(GETDATA_INCLUDEDIR getdata.h
	HINTS
	ENV GETDATA_DIR
	PATH_SUFFIXES include/getdata include
	PATHS ${kst_3rdparty_dir} ${GETDATA_INCLUDEDIR})

foreach(it ${PKGGETDATA_LIBRARIES})
	set(lib lib-NOTFOUND CACHE STRING "" FORCE)
	FIND_LIBRARY(lib ${it} 
		HINTS
		ENV GSL_DIR
		PATH_SUFFIXES lib
		PATHS ${kst_3rdparty_dir} ${PKGGETDATA_LIBRARY_DIRS})
	list(APPEND GETDATA_LIBRARIES ${lib})
endforeach()



IF(GETDATA_INCLUDEDIR AND GETDATA_INCLUDEDIR)
	SET(GETDATA_INCLUDE_DIR ${GETDATA_INCLUDEDIR})
	SET(getdata 1)
	message(STATUS "Found GetData:")
	message(STATUS "     includes : ${GETDATA_INCLUDE_DIR}")
	message(STATUS "     libraries: ${GETDATA_LIBRARIES}")
ELSE()
	MESSAGE(STATUS "Not found: Getdata")
ENDIF()



