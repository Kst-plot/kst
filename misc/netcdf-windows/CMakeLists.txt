cmake_minimum_required(VERSION 2.6.4)

project( netcdf)

set (TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)


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
		
	add_definitions(-D_FILE_OFFSET_BITS=64)
	include_directories(.)
	set(CMAKE_DEBUG_POSTFIX d)
endif()

set(NETCDF_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/installed)

add_subdirectory(libsrc)
add_subdirectory(cxx)



