
# use pkg to find the library name and pathes,
# but use this iformation in find_* only

include(FindPkgConfig)
pkg_check_modules(PKGGSL gsl)

if(NOT PKGGSL_LIBRARIES)
	set(PKGGSL_LIBRARIES gsl)
	if (UNIX)
		set(PKGGSL_LIBRARIES ${PKGGSL_LIBRARIES} m gslcblas)
	endif()
endif()

set(GSL_INCLUDEDIR GSL_INCLUDEDIR-NOTFOUND CACHE STRING "" FORCE)
find_path(GSL_INCLUDEDIR gsl_version.h
	HINTS
	ENV GSL_DIR
	PATH_SUFFIXES include/gsl include
	PATHS ${kst_3rdparty_dir} ${PKGGSL_INCLUDEDIR})

foreach(it ${PKGGSL_LIBRARIES})
	set(lib lib-NOTFOUND CACHE STRING "" FORCE)
	FIND_LIBRARY(lib ${it} 
		HINTS
		ENV GSL_DIR
		PATH_SUFFIXES lib
		PATHS ${kst_3rdparty_dir} ${PKGGSL_LIBRARY_DIRS})
	list(APPEND GSL_LIBRARIES ${lib})
endforeach()


if(GSL_INCLUDEDIR AND GSL_LIBRARIES)
	set(GSL_INCLUDE_DIR ${GSL_INCLUDEDIR} ${GSL_INCLUDEDIR}/..)
	set(gsl 1)
	message(STATUS "Found Gsl:")
	message(STATUS "     includes : ${GSL_INCLUDE_DIR}")
	message(STATUS "     libraries: ${GSL_LIBRARIES}")
else()
	message(STATUS "Not found: Gsl, set GSL_DIR")
endif()



