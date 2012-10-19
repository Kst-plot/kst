
set(CMAKE_SYSTEM_NAME Windows)

# MinGW on Ubuntu: i686-w64-mingw32/x86_64-w64-mingw32 or i586-mingw32msvc
set(TOOLNAME ${kst_cross})
set(TOOLCHAIN "${TOOLNAME}-")
set(CMAKE_C_COMPILER   "${TOOLCHAIN}gcc"     CACHE PATH "Mingw C compiler" FORCE)
set(CMAKE_CXX_COMPILER "${TOOLCHAIN}g++"     CACHE PATH "Mingw C++ compiler" FORCE)
set(CMAKE_RC_COMPILER  "${TOOLCHAIN}windres" CACHE PATH "Mingw rc compiler" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if(kst_qt4)
	# Cross-compiled Qt, branch cross-mingw from https://gitorious.org/~syntheticpp/qt/qt4
	set(QT_MINGW_DIR ${kst_qt4} CACHE PATH "Qt for Mingw" FORCE)
	set(CMAKE_FIND_ROOT_PATH /usr/${TOOLNAME} ${QT_MINGW_DIR} CACHE PATH "Mingw find root path" FORCE)
endif()


if(kst_3rdparty_dir)
    set(getdata 1 CACHE STRING "3rdparty" FORCE)
    set(GETDATA_INCLUDE_DIR ${kst_3rdparty_dir}/include CACHE STRING "3rdparty" FORCE)
    set(GETDATA_LIBRARIES   ${kst_3rdparty_dir}/lib/libgetdata++.a ${kst_3rdparty_dir}/lib/libgetdata.a CACHE STRING "3rdparty" FORCE)
    set(gsl 1 CACHE STRING "3rdparty" FORCE)
    set(GSL_INCLUDE_DIR     ${kst_3rdparty_dir}/include/gsl ${kst_3rdparty_dir}/include/gsl/.. CACHE STRING "3rdparty" FORCE)
    set(GSL_LIBRARIES       ${kst_3rdparty_dir}/lib/libgsl.a CACHE STRING "3rdparty" FORCE)
    set(netcdf 1 CACHE STRING "3rdparty" FORCE)
    set(NETCDF_INCLUDE_DIR  ${kst_3rdparty_dir}/include CACHE STRING "3rdparty" FORCE)
    set(NETCDF_LIBRARIES    ${kst_3rdparty_dir}/lib/libnetcdf_c++.a ${kst_3rdparty_dir}/lib/libnetcdf.a CACHE STRING "3rdparty" FORCE)
    set(matio 1 CACHE STRING "3rdparty" FORCE)
    set(MATIO_INCLUDE_DIR   ${kst_3rdparty_dir}/include ${kst_3rdparty_dir}/include/.. CACHE STRING "3rdparty" FORCE)
    set(MATIO_LIBRARIES     ${kst_3rdparty_dir}/lib/libmatio.a CACHE STRING "3rdparty" FORCE)
    set(cfitsio 1 CACHE STRING "3rdparty" FORCE)
    set(CFITSIO_INCLUDE_DIR ${kst_3rdparty_dir}/include/cfitsio;${kst_3rdparty_dir}/include/cfitsio/.. CACHE STRING "3rdparty" FORCE)
    set(CFITSIO_LIBRARIES   ${kst_3rdparty_dir}/lib/libcfitsio.a CACHE STRING "3rdparty" FORCE)
endif()
