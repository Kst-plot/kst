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

# Modern FindNetcdf.cmake
# Usage: find_package(Netcdf)
# Sets: NETCDF_FOUND, NETCDF_INCLUDE_DIR, NETCDF_LIBRARIES

include(FindPackageHandleStandardArgs)

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(NETCDF_PKG QUIET netcdf netcdf-cxx4)
endif()

# Find headers
find_path(NETCDF_INCLUDE_DIR
    NAMES netcdf.h
    HINTS ${NETCDF_PKG_INCLUDE_DIRS}
    PATHS ENV NETCDF_DIR
    PATH_SUFFIXES include
)

# Find libraries
find_library(NETCDF_LIBRARY_C
    NAMES netcdf
    HINTS ${NETCDF_PKG_LIBRARY_DIRS}
    PATHS ENV NETCDF_DIR
    PATH_SUFFIXES lib
)
find_library(NETCDF_LIBRARY_CPP
    NAMES netcdf_c++4
    HINTS ${NETCDF_PKG_LIBRARY_DIRS}
    PATHS ENV NETCDF_DIR
    PATH_SUFFIXES lib
)

set(NETCDF_LIBRARIES)
if(NETCDF_LIBRARY_C)
    list(APPEND NETCDF_LIBRARIES ${NETCDF_LIBRARY_C})
endif()
if(NETCDF_LIBRARY_CPP)
    list(APPEND NETCDF_LIBRARIES ${NETCDF_LIBRARY_CPP})
endif()

# Handle standard args and set _FOUND variable
find_package_handle_standard_args(Netcdf
    REQUIRED_VARS NETCDF_INCLUDE_DIR NETCDF_LIBRARIES
    HANDLE_COMPONENTS
)

if(NETCDF_FOUND)
    add_library(Netcdf::Netcdf UNKNOWN IMPORTED)
    set_target_properties(Netcdf::Netcdf PROPERTIES
        IMPORTED_LOCATION "${NETCDF_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${NETCDF_INCLUDE_DIR}"
    )
endif()