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

if(NOT TIFF_INCLUDEDIR)

if(NOT kst_cross)
	include(FindPkgConfig)
        pkg_check_modules(PKGTIFF QUIET TIFF)
endif()

if(NOT PKGTIFF_LIBRARIES)
        set(PKGTIFF_LIBRARIES tiff)
	if (UNIX)
                set(PKGTIFF_LIBRARIES ${PKGTIFF_LIBRARIES})
	endif()
endif()

set(TIFF_INCLUDEDIR TIFF_INCLUDEDIR-NOTFOUND CACHE STRING "" FORCE)
find_path(TIFF_INCLUDEDIR tiffio.h
	HINTS
        ENV TIFF_DIR
	PATH_SUFFIXES include
        PATHS ${kst_3rdparty_dir} ${PKGTIFF_INCLUDEDIR})

set(TIFF_LIBRARY_LIST)
if(kst_3rdparty_dir)
    list(APPEND PKGTIFF_LIBRARIES)
endif()
foreach(it ${PKGTIFF_LIBRARIES})
	set(lib lib-NOTFOUND CACHE STRING "" FORCE)
	FIND_LIBRARY(lib ${it} 
		HINTS
                ENV TIFF_DIR
		PATH_SUFFIXES lib
                PATHS ${kst_3rdparty_dir} ${PKGTIFF_LIBRARY_DIRS})
        list(APPEND TIFF_LIBRARY_LIST ${lib})
endforeach()
set(TIFF_LIBRARIES ${TIFF_LIBRARY_LIST} CACHE STRING "" FORCE)

endif()


if(TIFF_INCLUDEDIR AND TIFF_LIBRARIES)
        set(TIFF_INCLUDE_DIR ${TIFF_INCLUDEDIR} ${TIFF_INCLUDEDIR}/..)
        set(tiff 1)
        message(STATUS "Found TIFF (for 16 bit tiff grayscale images):")
        message(STATUS "     includes : ${TIFF_INCLUDE_DIR}")
        message(STATUS "     libraries: ${TIFF_LIBRARIES}")
else()
        message(STATUS "Not found: TIFF, set TIFF_DIR")
endif()

message(STATUS "")

