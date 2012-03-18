
rmdir /q/s build-mingw-release
mkdir build-mingw-release
cd build-mingw-release

cmake ..\kst\cmake -G"MinGW Makefiles" -Dkst_release=1 -Dkst_merge_files=0 -Dkst_3rdparty_download=1 -Dkst_install_prefix=Kst-%1 -Dkst_version_string=%1

mingw32-make -j
mingw32-make install
mingw32-make package
mingw32-make package_source

cd ..

