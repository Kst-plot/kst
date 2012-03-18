cmake_minimum_required(VERSION 2.6)

project(matio)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 

if(NOT ZLIB_INCLUDE_DIR OR NOT ZLIB_LIBRARY)
    find_package(ZLIB)
endif()
if(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
    message(STATUS "Building with zlib support")
    set(zlib 1)
    add_definitions(-DHAVE_ZLIB=1)
    include_directories(${ZLIB_INCLUDE_DIR})
endif()

if(NOT matio_dir)
    set(matio_dir ${CMAKE_SOURCE_DIR})
endif()

if(MSVC)
    configure_file(matioConfig.h.msvc.cmake   ${CMAKE_BINARY_DIR}/matioConfig.h   COPYONLY)
    configure_file(matio_pubConf.h.msvc.cmake ${CMAKE_BINARY_DIR}/matio_pubConf.h COPYONLY)
    configure_file(inttypes.h ${CMAKE_BINARY_DIR}/inttypes.h COPYONLY)
endif()
if(MINGW)
    configure_file(matioConfig.h.mingw.cmake   ${CMAKE_BINARY_DIR}/matioConfig.h   COPYONLY)
    configure_file(matio_pubConf.h.mingw.cmake ${CMAKE_BINARY_DIR}/matio_pubConf.h COPYONLY)
endif()


set(matio_src
    ${matio_dir}/src/snprintf.c
    ${matio_dir}/src/endian.c
    ${matio_dir}/src/io.c
    ${matio_dir}/src/read_data.c
    ${matio_dir}/src/mat5.c
    ${matio_dir}/src/mat4.c
    ${matio_dir}/src/mat.c
    ${matio_dir}/src/matvar_cell.c
    ${matio_dir}/src/matvar_struct.c)

if(zlib)
    list(APPEND matio_src ${matio_dir}/src/inflate.c)
endif()

set(matio_h
    ${CMAKE_BINARY_DIR}/matioConfig.h
    ${CMAKE_BINARY_DIR}/matio_pubconf.h
    ${matio_dir}/src/matio.h
    ${matio_dir}/src/matio_private.h 
    ${matio_dir}/src/mat4.h 
    ${matio_dir}/src/mat5.h 
    ${matio_dir}/src/mat73.h)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

include_directories(${matio_dir}/src ${CMAKE_BINARY_DIR})

add_library(matio STATIC ${matio_src} ${matio_h})

if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/installed)
endif()

install(FILES ${matio_dir}/src/matio.h ${CMAKE_BINARY_DIR}/matio_pubConf.h DESTINATION include)
install(TARGETS matio DESTINATION lib)


