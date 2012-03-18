cmake_minimum_required(VERSION 2.8)

project(netcdf)

if(netcdf_sources)
	set(TOP_DIR ${netcdf_sources})
else()
	set(TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
endif()


if(netcdf_install_prefix)
	set(CMAKE_INSTALL_PREFIX ${netcdf_install_prefix} CACHE PATH "User's choice for install prefix" FORCE)
	set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif()
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/INSTALLED CACHE PATH "Kst's default install prefix" FORCE)
endif()

message(STATUS)
message(STATUS "Using netCDF soures from: ${TOP_DIR}")
message(STATUS "Installing netCDF at    : ${CMAKE_INSTALL_PREFIX}")
message(STATUS)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -DNOMINMAX)

	# add here warnings which should produce an error /weXXXX
	set(MSVC_W_ERROR   "/we4101 /we4189")
	
	# add here warnings which should be disabled /wdXXXX
	set(MSVC_W_DISABLE "/wd4288 /wd4355 /wd4800 /wd4996 /wd4311 /wd4312 /wd4505 /wd4267 /wd4512 /wd4245 /wd4127 /wd4180 /wd4231")

	set(CMAKE_C_FLAGS_DEBUG
		"${CMAKE_C_FLAGS_DEBUG} /Wp64 ${MSVC_W_ERROR} ${MSVC_W_DISABLE}")
	set(CMAKE_C_FLAGS_RELEASE
		"${CMAKE_C_FLAGS_RELEASE} /Wp64 ${MSVC_W_ERROR} ${MSVC_W_DISABLE}")
	
	set(CMAKE_DEBUG_POSTFIX d)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-msvc.h ${CMAKE_BINARY_DIR}/config.h COPYONLY)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/inttypes.h ${CMAKE_BINARY_DIR}/inttypes.h COPYONLY)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/stdint.h ${CMAKE_BINARY_DIR}/stdint.h COPYONLY)
endif()

if(MINGW)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-mingw.h ${CMAKE_BINARY_DIR}/config.h COPYONLY)
endif()

add_definitions(-D_FILE_OFFSET_BITS=64)	
include_directories(${CMAKE_BINARY_DIR})

if (NOT CMAKE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/installed)
endif()

add_subdirectory(libsrc)
add_subdirectory(cxx)



