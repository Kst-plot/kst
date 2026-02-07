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


# Modern FindGetdata.cmake
# Usage: find_package(Getdata)
# Sets: GETDATA_FOUND, GETDATA_INCLUDE_DIR, GETDATA_LIBRARIES

include(FindPackageHandleStandardArgs)

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(GETDATA_PKG QUIET getdata)
endif()

# Find headers
find_path(GETDATA_INCLUDE_DIR
    NAMES getdata.h
    HINTS ${GETDATA_PKG_INCLUDE_DIRS}
    PATHS ENV GETDATA_DIR
    PATH_SUFFIXES include/getdata include
)

# Find libraries
find_library(GETDATA_LIBRARY
    NAMES getdata
    HINTS ${GETDATA_PKG_LIBRARY_DIRS}
    PATHS ENV GETDATA_DIR
    PATH_SUFFIXES lib
)
find_library(GETDATA_LIBRARY_CPP
    NAMES getdata++
    HINTS ${GETDATA_PKG_LIBRARY_DIRS}
    PATHS ENV GETDATA_DIR
    PATH_SUFFIXES lib
)

set(GETDATA_LIBRARIES)
if(GETDATA_LIBRARY)
    list(APPEND GETDATA_LIBRARIES ${GETDATA_LIBRARY})
endif()
if(GETDATA_LIBRARY_CPP)
    list(APPEND GETDATA_LIBRARIES ${GETDATA_LIBRARY_CPP})
endif()

# Handle standard args and set _FOUND variable
find_package_handle_standard_args(Getdata
    REQUIRED_VARS GETDATA_INCLUDE_DIR GETDATA_LIBRARIES
    HANDLE_COMPONENTS
)

# Create imported targets
if(Getdata_FOUND)
    add_library(Getdata::getdata UNKNOWN IMPORTED)
    set_target_properties(Getdata::getdata PROPERTIES
        IMPORTED_LOCATION "${GETDATA_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GETDATA_INCLUDE_DIR}"
    )
    if(GETDATA_LIBRARY_CPP)
        add_library(Getdata::getdata++ UNKNOWN IMPORTED)
        set_target_properties(Getdata::getdata++ PROPERTIES
            IMPORTED_LOCATION "${GETDATA_LIBRARY_CPP}"
            INTERFACE_INCLUDE_DIRECTORIES "${GETDATA_INCLUDE_DIR}"
        )
    endif()
endif()

