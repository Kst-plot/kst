# ***************************************************************************
# *                                                                         *
# *   Copyright : (C) 2026 C. Barth Netterfield  						    *
# *   email     : netterfield@astro.utoronto.ca                             *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# ***************************************************************************

# Modern FindCFITSIO.cmake
# Usage: find_package(CFITSIO)
# Sets: CFITSIO_FOUND, CFITSIO_INCLUDE_DIR, CFITSIO_LIBRARIES

include(FindPackageHandleStandardArgs)

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(CFITSIO_PKG QUIET cfitsio)
endif()

# Find headers
find_path(CFITSIO_INCLUDE_DIR
    NAMES fitsio.h
    HINTS ${CFITSIO_PKG_INCLUDE_DIRS}
    PATHS ENV CFITSIO_DIR
    PATH_SUFFIXES include
)

# Find libraries
find_library(CFITSIO_LIBRARY
    NAMES cfitsio
    HINTS ${CFITSIO_PKG_LIBRARY_DIRS}
    PATHS ENV CFITSIO_DIR
    PATH_SUFFIXES lib
)

set(CFITSIO_LIBRARIES)
if(CFITSIO_LIBRARY)
    list(APPEND CFITSIO_LIBRARIES ${CFITSIO_LIBRARY})
endif()

# Handle standard args and set _FOUND variable
find_package_handle_standard_args(CFITSIO
    REQUIRED_VARS CFITSIO_INCLUDE_DIR CFITSIO_LIBRARIES
    HANDLE_COMPONENTS
)

if(CFITSIO_FOUND)
    add_library(CFITSIO::CFITSIO UNKNOWN IMPORTED)
    set_target_properties(CFITSIO::CFITSIO PROPERTIES
        IMPORTED_LOCATION "${CFITSIO_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${CFITSIO_INCLUDE_DIR}"
    )
endif()
