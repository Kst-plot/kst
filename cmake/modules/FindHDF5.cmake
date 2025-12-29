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

# copied from findnetcdf.cmake
if(NOT HDF5_INCLUDEDIR)

  if(NOT kst_cross)
    include(FindPkgConfig)
    pkg_check_modules(HDF5 QUIET hdf5)
  endif()

  if(HDF5_INCLUDEDIR AND HDF5_LIBRARIES)
    FIND_LIBRARY(HDF5_LIBRARY_CPP hdf5_cpp
      HINTS ${HDF5_LIBRARY_DIRS})
    set(HDF5_LIBRARY_C -L${HDF5_LIBRARY_DIRS} ${HDF5_LIBRARIES} CACHE STRING "" FORCE)
  else()
    set(HDF5_INCLUDEDIR HDF5_INCLUDEDIR-NOTFOUND CACHE STRING "" FORCE)
    FIND_PATH(HDF5_INCLUDEDIR H5Cpp.h
      HINTS
      ENV HDF5_DIR
      PATH_SUFFIXES include/hdf5 include/hdf5/serial include
      PATHS
      ${kst_3rdparty_dir}
      ~/Library/Frameworks
      /Library/Frameworks
      )

    macro(find_hdf5_lib var libname)
      FIND_LIBRARY(${var} ${libname}
        HINTS
        ENV HDF5_DIR
        PATH_SUFFIXES lib hdf5/serial lib/x86_64-linux-gnu/hdf5/serial
        PATHS ${kst_3rdparty_dir})
    endmacro()

    find_hdf5_lib(hdf5_c         hdf5)
    find_hdf5_lib(hdf5_c_debug   hdf5d)
    find_hdf5_lib(hdf5_cpp       hdf5_c++)
    find_hdf5_lib(hdf5_cpp_debug hdf5_c++d)

    if (hdf5_cpp STREQUAL "hdf5_cpp-NOTFOUND")
      find_hdf5_lib(hdf5_cpp       hdf5_cpp)
    endif()

    if(hdf5_c AND hdf5_c_debug)
      set(HDF5_LIBRARY_C optimized ${hdf5_c} debug ${hdf5_c_debug} CACHE STRING "" FORCE)
    endif()
    if(hdf5_cpp AND hdf5_cpp_debug)
       set(HDF5_LIBRARY_CPP optimized ${hdf5_cpp} debug ${hdf5_cpp_debug} CACHE STRING "" FORCE)
    endif()

    if(NOT MSVC)
      # only msvc needs debug and release
      set(HDF5_LIBRARY_C   ${hdf5_c}   CACHE STRING "" FORCE)
      set(HDF5_LIBRARY_CPP ${hdf5_cpp} CACHE STRING "" FORCE)
    endif()
  endif()
endif()

  #message(STATUS "HDF5: ${HDF5_INCLUDEDIR}")
  #message(STATUS "HDF5: ${HDF5_LIBRARY_C}")
  #message(STATUS "HDF5: ${HDF5_LIBRARY_CPP}")
  IF(HDF5_INCLUDEDIR AND HDF5_LIBRARY_C AND HDF5_LIBRARY_CPP)
    SET(HDF5_LIBRARIES ${HDF5_LIBRARY_CPP} ${HDF5_LIBRARY_C})
    SET(HDF5_INCLUDE_DIR ${HDF5_INCLUDEDIR})
    SET(hdf5 1)
    message(STATUS "Found HDF5:")
    message(STATUS "     includes : ${HDF5_INCLUDE_DIR}")
    message(STATUS "     libraries: ${HDF5_LIBRARIES}")
  ELSE()
    MESSAGE(STATUS "Not found: HDF5.")
      MESSAGE(STATUS "      If HDF5 is installed outside the CMake search path,")
      MESSAGE(STATUS "      set the environmental variable HDF5_DIR to the")
      MESSAGE(STATUS "      HDF5 install prefix.")
  ENDIF()

  message(STATUS "")

