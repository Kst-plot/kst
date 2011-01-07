cmake_minimum_required(VERSION 2.8)

project(Kst)

set(KST_VERSION 2.0.3)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
include(KstMacros)

message(STATUS)
message(STATUS "Build options:")
message(STATUS)

kst_option(release "Build release version" OFF all)
kst_option(merge_files "Merge files to speedup build" OFF all)
kst_option(merge_rebuild "Rebuild generated files from merged files build" OFF all)
kst_option(verbose "Make verbose makefiles" OFF all)
kst_option(install_prefix "Install path for Kst, using a default if not set" OFF all)
kst_option(console "Open console on Windows" OFF win)

message(STATUS)

# Find 3rd party libraries
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
if(MINGW)
	# seems there is a bug in FindQt4
	macro(fix_it)
		foreach(_it ${ARGN})
			if (QT_${_it}_LIBRARY_DEBUG AND QT_${_it}_LIBRARY_RELEASE)
				set(QT_${_it}_LIBRARY debug ${QT_${_it}_LIBRARY_DEBUG} optimized ${QT_${_it}_LIBRARY_RELEASE} CACHE STRING "Fixed ${_it}" FORCE)
			endif()
		endforeach()
	endmacro()
	fix_it(QTCORE QTGUI QTXML QTSVG QTOPENGL)
endif()

message(STATUS)
message(STATUS "3rd party libs for plugins--------------------")
find_package(Getdata)
find_package(Gsl)
find_package(Netcdf)
message(STATUS "----------------------------------------------")
message(STATUS)


# React on options
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

if(kst_install_prefix)
	set(CMAKE_INSTALL_PREFIX ${install_prefix} CACHE PATH "User's choice for install prefix" FORCE)
	set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif()
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/INSTALLED CACHE PATH "Kst's default install prefix" FORCE)
endif()

if(NOT kst_console)
	set(kst_win32 WIN32)
	set(kst_qtmain_library ${QT_QTMAIN_LIBRARY})
endif()



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 


if(NOT KST_DIR)
	set(KST_DIR ${CMAKE_SOURCE_DIR}/..)
endif()

if(MSVC)
	add_definitions(-D_USE_MATH_DEFINES)
endif()

if(WIN32)
	set(CMAKE_DEBUG_POSTFIX d)
endif()

configure_file(${KST_DIR}/cmake/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

include_directories(
	${CMAKE_BINARY_DIR}
	${QT_INCLUDES}
	)


message(STATUS "Build summary----------------------------------")
message(STATUS "Version          : ${KST_VERSION}")
message(STATUS "Path to source   : ${KST_DIR}")
message(STATUS "Installation path: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "-----------------------------------------------")
message(STATUS)

add_subdirectory(src)

