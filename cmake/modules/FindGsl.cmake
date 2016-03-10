# ***************************************************************************
# *                                                                         *
# *   Copyright : (C) 2010 The University of Toronto                        *
# *   email     : netterfield@astro.utoronto.ca                             *
# *                                                                         *
# *   Copyright : (C) 2010 Peter Kümmel                                     *
# *   email     : syntheticpp@gmx.net                                       *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# ***************************************************************************

# use pkg to find the library name and pathes,
# but use this iformation in find_* only
if(NOT GSL_INCLUDEDIR)

if(NOT kst_cross)
	include(FindPkgConfig)
	pkg_check_modules(PKGGSL QUIET gsl)
endif()

if(NOT PKGGSL_LIBRARIES)
	set(PKGGSL_LIBRARIES gsl)
	if (UNIX AND NOT kst_cross)
		set(PKGGSL_LIBRARIES ${PKGGSL_LIBRARIES} m gslcblas)
	endif()
endif()

set(GSL_INCLUDEDIR GSL_INCLUDEDIR-NOTFOUND CACHE STRING "" FORCE)
find_path(GSL_INCLUDEDIR gsl_version.h
	HINTS
	ENV GSL_DIR
	PATH_SUFFIXES include/gsl include
	PATHS ${kst_3rdparty_dir} ${PKGGSL_INCLUDEDIR})

set(GSL_LIBRARY_LIST)
foreach(it ${PKGGSL_LIBRARIES})
	set(lib lib-NOTFOUND CACHE STRING "" FORCE)
	FIND_LIBRARY(lib ${it} 
		HINTS
		ENV GSL_DIR
		PATH_SUFFIXES lib
		PATHS ${kst_3rdparty_dir} ${PKGGSL_LIBRARY_DIRS})
	list(APPEND GSL_LIBRARY_LIST ${lib})
endforeach()
set(GSL_LIBRARIES ${GSL_LIBRARY_LIST} CACHE STRING "" FORCE)

endif()

if(GSL_INCLUDEDIR AND GSL_LIBRARIES)
	set(GSL_INCLUDE_DIR ${GSL_INCLUDEDIR} ${GSL_INCLUDEDIR}/..)
	set(gsl 1)
    message(STATUS "Found GSL:")
	message(STATUS "     includes : ${GSL_INCLUDE_DIR}")
	message(STATUS "     libraries: ${GSL_LIBRARIES}")
else()
	message(STATUS "Not found: GSL.")
    MESSAGE(STATUS "      If GSL is installed outside the CMake search path,")
    MESSAGE(STATUS "      set the environmental variable GSL_DIR to the")
    MESSAGE(STATUS "      GSL install prefix.")
endif()

message(STATUS "")

