cmake_minimum_required(VERSION 2.8)

project(Kst)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
include(KstMacros)

message(STATUS "")
message(STATUS "Build options:")
message(STATUS "")
kst_option(release "Build release version" OFF all)
kst_option(merge_files "Merge files to speedup build" OFF all)
kst_option(merge_rebuild "Rebuild generated files from merged files build" OFF all)
kst_option(verbose "Make verbose makefiles" OFF all)
message(STATUS "")


if(kst_release)
	set(CMAKE_BUILD_TYPE Release)
else()
	set(CMAKE_BUILD_TYPE Debug)
endif()

if(kst_verbose)
	set(CMAKE_VERBOSE_MAKEFILE 1)
else()
	set(CMAKE_VERBOSE_MAKEFILE 0)
endif()

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

message(STATUS "")
message(STATUS "3rd party libs for plugins:")
message(STATUS "")
find_package(Getdata)
find_package(Gsl)
find_package(Netcdf)
message(STATUS "")

set(KST_VERSION 2.0.3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 


if(NOT KST_DIR)
	set(KST_DIR ${CMAKE_SOURCE_DIR}/..)
endif()

if(MSVC)
	add_definitions(-D_USE_MATH_DEFINES)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/INSTALLED)
endif()

configure_file(${KST_DIR}/cmake/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

include_directories(
	${CMAKE_BINARY_DIR}
	${QT_INCLUDES}
	)


add_subdirectory(src)

