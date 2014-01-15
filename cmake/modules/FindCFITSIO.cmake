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

if(NOT CFITSIO_INCLUDEDIR)

if(NOT kst_cross)
	include(FindPkgConfig)
	pkg_check_modules(cfitsio QUIET cfitsio)
endif()

if(CFITSIO_INCLUDEDIR AND CFITSIO_LIBRARIES)
	set(CFITSIO_LIBRARY -L${CFITSIO_LIBRARY_DIRS} ${CFITSIO_LIBRARIES})
else()
	set(CFITSIO_INCLUDEDIR CFITSIO_INCLUDEDIR-NOTFOUND CACHE STRING "" FORCE)
	FIND_PATH(CFITSIO_INCLUDEDIR fitsio.h
		HINTS
		ENV CFITSIO_DIR
		PATH_SUFFIXES include include/cfitsio include/libcfitsio0
		PATHS ${kst_3rdparty_dir}
		)
	FIND_LIBRARY(CFITSIO_LIBRARIES cfitsio 
		HINTS
		ENV CFITSIO_DIR
		PATH_SUFFIXES lib
		PATHS ${kst_3rdparty_dir}
		)
endif()
endif()

#message(STATUS "CFITSIO: ${CFITSIO_INCLUDEDIR}")
#message(STATUS "CFITSIO: ${CFITSIO_LIBRARIES}")
IF(CFITSIO_INCLUDEDIR AND CFITSIO_LIBRARIES)
	SET(CFITSIO_INCLUDE_DIR ${CFITSIO_INCLUDEDIR} ${CFITSIO_INCLUDEDIR}/..)
	if (UNIX)
		SET(CFITSIO_LIBRARIES ${CFITSIO_LIBRARIES} m)
	endif()
	SET(CFITSIO 1)
	SET(cfitsio 1)
	message(STATUS "Found CFITSIO:")
	message(STATUS "     includes : ${CFITSIO_INCLUDE_DIR}")
	message(STATUS "     libraries: ${CFITSIO_LIBRARIES}")
ELSE()
	MESSAGE(STATUS "Not found: CFITSIO, set CFITSIO_DIR")
ENDIF()
message(STATUS "")


