This folder contains files needed for building netCDF on Windows.
(netCDF 3 only, no HDF5 support)

Build instructions:

- Get netCDF source code from http://www.unidata.ucar.edu/software/netcdf/
  (latest tested release: 4.1.1, 2010/05) 

- Extract netCDF sources, eg. c:\netcdf-sources
- Copy this netcdf-windows folder into the netCDF sources, eg. c:\netcdf-sources\netcdf-windows
- Make a build folder, eg. c:\netcdf-build

- Build netCDF in this folder with CMake (http://www.cmake.org):
  * cd c:\netcdf-build
  * MSVC : cmake ..\netcdf-sources\netcdf-windows -G"Visual Studio 9 2008" 
    MinGW: cmake ..\netcdf-sources\netcdf-windows -G"MinGW Makefiles"
  * MSVC : build and install (build INSTALL project) with Visual Studio
    MinGW: mingw32-make & mingw32-make install

- Set environment variable NETCDFDIR to 'installed' folder:
    set NETCDFDIR=c:\netcdf-build\installed
   
- qmake will detect netCDF and enables netCDF on Windows.


TODO: Do real checks for config.h, no HDF5 support

