cmake_minimum_required(VERSION 2.6)

project( getdata )


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 


set(GETDATA_DIR ${CMAKE_SOURCE_DIR}/..)


macro(GD_FILES name folder)
	file(GLOB ${name}_folder_sources ${GETDATA_DIR}/${folder}/*.c) 
	file(GLOB ${name}_folder_sources_cpp ${GETDATA_DIR}/${folder}/*.cpp)
	file(GLOB ${name}_folder_headers ${GETDATA_DIR}/${folder}/*.h) 
	set(${name}_sources ${${name}_sources} ${${name}_folder_sources} ${${name}_folder_sources_cpp})
	set(${name}_headers ${${name}_headers} ${${name}_folder_headers})
	include_directories(${CMAKE_SOURCE_DIR}/${folder})
endmacro()


if(MSVC)
	# ALPHA quality!
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
	add_definitions(
		-D__MSVCRT__
		-DSTDC_HEADERS
		-DHAVE_INTTYPES_H
		-DHAVE_IO_H
		-Dsnprintf=_snprintf
		-Dfsync=_commit
		-Dmode_t=int
		-Dssize_t=size_t
		-DGD_USE_CPP
		-DHAVE_STDINT_H
		)
	set(CMAKE_DEBUG_POSTFIX d)
        configure_file(inttypes.h ${CMAKE_CURRENT_BINARY_DIR}/inttypes.h COPYONLY)
endif()

if(MINGW)
	# ALPHA quality!
	add_definitions(
			-D__MSVCRT__
			-DSTDC_HEADER
			-DSTDC_HEADER
			-DHAVE_IO_H
			-DHAVE_STDINT_H
			-DHAVE_COMPLEX_H
			-DHAVE_OFF64_T
			-DSTDC_HEADERS
			-DHAVE_LIBGEN_H
			-DHAVE_STRTOLL
			-DPACKAGE_BUGREPORT=""
			-DPACKAGE_NAME=""
			-DPACKAGE_VERSION=""
			-DHAVE_DIRENT_H
			-DHAVE__COMMIT
			)
endif()


configure_file(${GETDATA_DIR}/src/getdata.h.in ${CMAKE_BINARY_DIR}/getdata.h COPYONLY)

include_directories(${GETDATA_DIR}/src ${CMAKE_CURRENT_BINARY_DIR})

if (NOT CMAKE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/installed)
endif()

add_subdirectory(src)
add_subdirectory(bindings)



