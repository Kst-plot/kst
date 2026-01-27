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

# Modern FindMatio.cmake
# Usage: find_package(Matio)
# Sets: MATIO_FOUND, MATIO_INCLUDE_DIR, MATIO_LIBRARIES

include(FindPackageHandleStandardArgs)

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(MATIO_PKG QUIET matio)
endif()

# Find headers
find_path(MATIO_INCLUDE_DIR
    NAMES matio.h
    HINTS ${MATIO_PKG_INCLUDE_DIRS}
    PATHS ENV MATIO_DIR
    PATH_SUFFIXES include
)

# Find libraries
find_library(MATIO_LIBRARY
    NAMES matio
    HINTS ${MATIO_PKG_LIBRARY_DIRS}
    PATHS ENV MATIO_DIR
    PATH_SUFFIXES lib
)

set(MATIO_LIBRARIES)
if(MATIO_LIBRARY)
    list(APPEND MATIO_LIBRARIES ${MATIO_LIBRARY})
endif()

# Handle standard args and set _FOUND variable
find_package_handle_standard_args(Matio
    REQUIRED_VARS MATIO_INCLUDE_DIR MATIO_LIBRARIES
    HANDLE_COMPONENTS
)

if(Matio_FOUND)
    add_library(Matio::Matio UNKNOWN IMPORTED)
    set_target_properties(Matio::Matio PROPERTIES
        IMPORTED_LOCATION "${MATIO_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${MATIO_INCLUDE_DIR}"
    )
endif()