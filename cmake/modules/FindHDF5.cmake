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

# Modern FindHDF5.cmake
# Usage: find_package(HDF5)
# Sets: HDF5_FOUND, HDF5_INCLUDE_DIR, HDF5_LIBRARIES

include(FindPackageHandleStandardArgs)

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(HDF5_PKG QUIET hdf5)
endif()

# Find headers
find_path(HDF5_INCLUDE_DIR
    NAMES hdf5.h H5Cpp.h
    HINTS ${HDF5_PKG_INCLUDE_DIRS}
    PATHS ENV HDF5_DIR
    PATH_SUFFIXES include/hdf5/serial include/hdf5
)

# Find libraries
find_library(HDF5_LIBRARY
    NAMES hdf5
    HINTS ${HDF5_PKG_LIBRARY_DIRS}
    PATHS ENV HDF5_DIR
    PATH_SUFFIXES lib
)
find_library(HDF5_LIBRARY_CPP
    NAMES hdf5_cpp
    HINTS ${HDF5_PKG_LIBRARY_DIRS}
    PATHS ENV HDF5_DIR
    PATH_SUFFIXES lib
)

set(HDF5_LIBRARIES)
if(HDF5_LIBRARY)
    list(APPEND HDF5_LIBRARIES ${HDF5_LIBRARY})
endif()
if(HDF5_LIBRARY_CPP)
    list(APPEND HDF5_LIBRARIES ${HDF5_LIBRARY_CPP})
endif()

# Handle standard args and set _FOUND variable
find_package_handle_standard_args(HDF5
    REQUIRED_VARS HDF5_INCLUDE_DIR HDF5_LIBRARIES
    HANDLE_COMPONENTS
)

if(HDF5_FOUND)
    # Create target for C library
    if(HDF5_LIBRARY)
        add_library(HDF5::HDF5 UNKNOWN IMPORTED)
        set_target_properties(HDF5::HDF5 PROPERTIES
            IMPORTED_LOCATION "${HDF5_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${HDF5_INCLUDE_DIR}"
        )
    endif()

    # Create target for C++ library (if found)
    if(HDF5_LIBRARY_CPP)
        add_library(HDF5::HDF5_CPP UNKNOWN IMPORTED)
        set_target_properties(HDF5::HDF5_CPP PROPERTIES
            IMPORTED_LOCATION "${HDF5_LIBRARY_CPP}"
            INTERFACE_INCLUDE_DIRECTORIES "${HDF5_INCLUDE_DIR}"
        )
    endif()
endif()