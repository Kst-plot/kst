This folder contains files needed for building GSL on Windows.


Build instructions:

- Get GSL source code from http://www.gnu.org/software/gsl/
  (latest tested release: 1.14, 2010/04) 
  
- Copy CMakeLists.txt and config.h to the GSL folder.

- Patch GSL with gsl-msvc.patch or gsl-mingw.patch (for MinGW 4.4.0).

- Extract GSL sources, eg. c:\gsl-sources
- Make a build folder, eg. c:\gsl-build

- Build GSL in this folder with CMake (http://www.cmake.org):
  * cd c:\gsl-build
  * MSVC : cmake ..\gst-sources -G"Visual Studio 9 2008" 
    MinGW: cmake ..\gst-sources -G"MinGW Makefiles"
  * MSVC : build and install (build INSTALL project) with Visual Studio
    MinGW: mingw32-make & mingw32-make install

- Set environment variable GSLDIR to 'installed' folder:
    set GSLDIR=c:\gsl-build\installed
   
- qmake will detect GSL and enables GSL on Windows.


TODO: Do real checks for config.h 
