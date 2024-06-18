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

# copied from FindGsl.cmake

if(NOT HDF5_INCLUDEDIR OR HDF5_INCLUDEDIR STREQUAL "HDF5_INCLUDEDIR-NOTFOUND") 

if(NOT kst_cross)
	include(FindPkgConfig)
        pkg_check_modules(PKGHDF5 QUIET hdf5 hdf5-cpp)
endif()

if(NOT PKGHDF5_LIBRARIES)
	message("NO PKGHDF5_LIBRARIES found")
	set(PKGHDF5_LIBRARIES hdf5_cpp)
	#if (UNIX)
	#	list(APPEND PKGHDF5_LIBRARIES ${PKGHDF5_LIBRARIES})
	#endif()
endif()

set(HDF5_DIR /usr/include/hdf5)

set(HDF5_INCLUDEDIR HDF5_INCLUDEDIR-NOTFOUND CACHE STRING "" FORCE)
find_path(HDF5_INCLUDEDIR H5Cpp.h
	HINTS
	ENV HDF5_DIR
	PATH_SUFFIXES serial
	PATHS ${kst_3rdparty_dir} ${PKGHDF5_INCLUDEDIR} ${HDF5_DIR})

set(HDF5_LIBRARY_LIST)
if(kst_3rdparty_dir)
	list(APPEND PKGHDF5_LIBRARIES)
endif()
foreach(it ${PKGHDF5_LIBRARIES})
	set(lib lib-NOTFOUND CACHE STRING "" FORCE)
	FIND_LIBRARY(lib ${it} 
		HINTS
		ENV HDF5_DIR
		PATH_SUFFIXES lib
		PATHS ${kst_3rdparty_dir} ${PKGHDF5_LIBRARY_DIRS})
	list(APPEND HDF5_LIBRARY_LIST ${lib})
endforeach()
set(HDF5_LIBRARIES ${HDF5_LIBRARY_LIST} CACHE STRING "" FORCE)

endif()

if(HDF5_INCLUDEDIR AND HDF5_LIBRARIES)
	set(HDF5_INCLUDE_DIR ${HDF5_INCLUDEDIR} ${HDF5_INCLUDEDIR}/..)
        set(hdf5 1)
	message(STATUS "Found HDF5 (for hdf5 data files):")
	message(STATUS "     includes : ${HDF5_INCLUDE_DIR}")
	message(STATUS "     libraries: ${HDF5_LIBRARIES}")
	message(STATUS "THIS IS A TEST")
else()
	message(STATUS "Not found: HDF5, set HDF5_DIR")
endif()

message(STATUS "")

