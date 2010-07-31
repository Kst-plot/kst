::TODO remove hardcoded paths
:: TODO specify path for sources
:: TODO copy cmake files to sources

set KSTDEPS=q:\kst-win32-deps-mingw
rmdir /s/q %KSTDEPS

del CMakeCache.txt
cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%KSTDEPS% Q:\netcdf\netcdf-4.1.1\netcdf-4.1.1\netcdf-windows
mingw32-make install


del CMakeCache.txt
cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%KSTDEPS% Q:\getdata\getdata\cmake
mingw32-make install


del CMakeCache.txt
cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%KSTDEPS% Q:\gsl\gsl-1.14\gsl-1.14
mingw32-make install



set GETDATADIR=%KSTDEPS%
set NETCDFDIR=%KSTDEPS%
set GFLDIR=%KSTDEPS%

qmake ..\kst\kst.pro CONFIG+=release -r

mingw32-make
