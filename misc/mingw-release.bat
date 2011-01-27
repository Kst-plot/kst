::TODO remove hardcoded paths
:: TODO specify path for sources
:: TODO copy cmake files to sources

set KSTDEPS=q:\kst-win32-deps-mingw
rmdir /s/q %KSTDEPS%

del CMakeCache.txt
cmake ..\kst\misc\netcdf-windows -Dnetcdf_sources=Q:/netcdf/netcdf-4.1.1/netcdf-4.1.1 -G"MinGW Makefiles" -DCMAKE_VERBOSE_MAKEFILE=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%KSTDEPS%
mingw32-make install


del CMakeCache.txt
cmake Q:\getdata\getdata\cmake -G"MinGW Makefiles" -DCMAKE_VERBOSE_MAKEFILE=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%KSTDEPS%
mingw32-make install


del CMakeCache.txt
cmake ..\kst\misc\gsl-windows Q:\gsl\gsl-1.14\gsl-1.14 -G"MinGW Makefiles" -DCMAKE_VERBOSE_MAKEFILE=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%KSTDEPS%
mingw32-make install

del CMakeCache.txt
cmake ..\kst\misc\cfitsio-windows -DCFIT_DIR=Q:/cfits/cfit3250 -G"MinGW Makefiles" -DCMAKE_VERBOSE_MAKEFILE=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%KSTDEPS% 
mingw32-make install


set GETDATA_DIR=%KSTDEPS%
set NETCDF_DIR=%KSTDEPS%
set GSL_DIR=%KSTDEPS%
set CFITSIO_DIR=%KSTDEPS%

del CMakeCache.txt
rmdir /s/q bin
rmdir /s/q lib
rmdir /s/q plugin

cmake ..\kst\cmake -G"MinGW Makefiles" -Dkst_release=1 -Dkst_merge_files=1 

mingw32-make
mingw32-make install
