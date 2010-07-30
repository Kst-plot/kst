This folder contains files needed for building GetData on Windows.

Build instructions:

- Get the  source code from http://getdata.sourceforge.net/
  (Tested Jul 30, 2010: svn co http://getdata.svn.sourceforge.net/svnroot/getdata/trunk/getdata)
 
- Copy cmake folder folder into the getdata sources, eg. c:\getdata\cmake
- Make a build folder, eg. c:\getdata-build

- Build getdata in this folder with CMake (http://www.cmake.org):
  * cd c:\getdata-build
    MinGW: cmake ..\getdata\cmake -G"MinGW Makefiles"
  * Build and install:
    MinGW: mingw32-make & mingw32-make install

- Set environment variable GETDATADIR to 'installed' folder:
    set GETDATADIR=c:\getdata-build\installed
   
- qmake will detect getdata and enables DirFile support on Windows.


TODO: MSVC build

