cmake_minimum_required(VERSION 2.8)

# figure out if we use MinGW with Ninja: call with -DCC=gcc
if(WIN32 AND CMAKE_GENERATOR MATCHES Ninja AND CC MATCHES gcc)
    find_file(mingw_make mingw32-make.exe)
    get_filename_component(mingw_bin_dir ${mingw_make} PATH)
    set(CMAKE_RC_COMPILER ${mingw_bin_dir}/windres.exe)
    message("CMAKE_RC_COMPILER: ${CMAKE_RC_COMPILER}")
endif()

project(3rdparty)

include(ExternalProject)

set(build_dir ${CMAKE_BINARY_DIR}/3rdparty)
if(kst_3rdparty_dir)
    set(install_dir ${kst_3rdparty_dir})
else()
    set(install_dir ${build_dir}/installed)
endif()
set(generator -G${CMAKE_GENERATOR})
set(install_prefix -DCMAKE_INSTALL_PREFIX=${install_dir})
if(CMAKE_BUILD_TYPE)
    set(build_type -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
endif()
message(STATUS "3rdparty build type: ${build_type}")
if(MINGW AND CMAKE_GENERATOR MATCHES Ninja)
    set(rc_compiler -DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER})
    set(install_target install\\strip)
else()
    set(install_target install/strip)
endif()

# GETDATA
set(getdata_ver 0.7.3) 
ExternalProject_Add(getdata
                    PREFIX ${build_dir}/getdata
                    #URL http://sourceforge.net/projects/getdata/files/getdata/${getdata_ver}/getdata_win-${getdata_ver}.zip # broken with mingw
                    URL http://sourceforge.net/projects/kst/files/3rdparty/getdata_win-0.7.3-mingw-fixed.zip
                    CONFIGURE_COMMAND cmake ../getdata/cmake
                                            ${generator} ${rc_compiler}
                                            ${install_prefix}
                                            ${build_type}
                    BUILD_IN_SOURCE OFF
                    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} ${install_target})
                    
set(GETDATA_INCLUDEDIR ${install_dir}/include PARENT_SCOPE)
set(GETDATA_LIBRARIES  ${install_dir}/lib/libgetdata++.a  ${install_dir}/lib/libgetdata.a PARENT_SCOPE)


# ZLIB needed by MATIO
ExternalProject_Add(zlib
                    PREFIX ${build_dir}/zlib
                    URL http://sourceforge.net/projects/kst/files/3rdparty/zlib-develop-mingw-fixed.zip
                    CONFIGURE_COMMAND cmake ../zlib 
                                            -DBUILD_SHARED_LIBS=0
                                            ${generator} ${rc_compiler}
                                            ${install_prefix}
                                            ${build_type}
                    BUILD_IN_SOURCE OFF
                    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} ${install_target})


# MATIO
ExternalProject_Add(matio
                    PREFIX ${build_dir}/matio
                    URL http://sourceforge.net/projects/kst/files/3rdparty/matio-2012.03.14.zip
                    CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/matio
                                            -Dmatio_dir=${build_dir}/matio/src/matio
                                            -DZLIB_INCLUDE_DIR=${install_dir}/include
                                            -DZLIB_LIBRARY=${install_dir}/lib/libzlibstatic.a
                                            ${generator} ${rc_compiler}
                                            ${install_prefix}
                                            ${build_type}
                    BUILD_IN_SOURCE OFF
                    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} ${install_target})
add_dependencies(matio zlib)

set(MATIO_INCLUDEDIR ${install_dir}/include PARENT_SCOPE)
set(MATIO_LIBRARIES  ${install_dir}/lib/libmatio.a ${install_dir}/lib/libzlibstatic.a PARENT_SCOPE)


# NETCDF
ExternalProject_Add(netcdf
                    PREFIX ${build_dir}/netcdf
                    URL ftp://ftp.unidata.ucar.edu/pub/netcdf/netcdf-4.1.1.tar.gz
                    CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/netcdf
                                            -Dnetcdf_sources=${build_dir}/netcdf/src/netcdf
                                            ${generator} ${rc_compiler}
                                            ${install_prefix}
                                            ${build_type}
                    BUILD_IN_SOURCE OFF
                    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} ${install_target})

set(NETCDF_INCLUDEDIR  ${install_dir}/include PARENT_SCOPE)
set(NETCDF_LIBRARY_C   ${install_dir}/lib/libnetcdf.a PARENT_SCOPE)
set(NETCDF_LIBRARY_CPP ${install_dir}/lib/libnetcdf_c++.a PARENT_SCOPE)


# CFITSIO
ExternalProject_Add(cfitsio
                    PREFIX ${build_dir}/cfitsio
                    URL ftp://heasarc.gsfc.nasa.gov/software/fitsio/c/cfit3250.zip
                    CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/cfitsio
                                            -DCFIT_DIR=${build_dir}/cfitsio/src/cfitsio
                                            ${generator} ${rc_compiler}
                                            ${install_prefix}
                                            ${build_type}
                    BUILD_IN_SOURCE OFF
                    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} ${install_target})
                    
set(CFITSIO_INCLUDEDIR ${install_dir}/include/cfitsio PARENT_SCOPE)
set(CFITSIO_LIBRARIES  ${install_dir}/lib/libcfitsio.a PARENT_SCOPE)


if(DISABLE) # linker errors
# GSL
ExternalProject_Add(gsl
                    PREFIX ${build_dir}/gsl
                    #URL ftp://ftp.gnu.org/gnu/gsl/gsl-1.14.tar.gz
                    URL http://sourceforge.net/projects/kst/files/3rdparty/gsl-1.14-patched.zip
                    CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/gsl
                                            -DGSL_DIR=${build_dir}/gsl/src/gsl
                                            ${generator} ${rc_compiler}
                                            ${install_prefix}
                                            ${build_type}
                    BUILD_IN_SOURCE OFF
                    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install)
                    
set(GSL_INCLUDEDIR ${install_dir}/include PARENT_SCOPE)
set(GSL_LIBRARIES  ${install_dir}/lib/libgsl.a PARENT_SCOPE)

endif()



