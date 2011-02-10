
cmake ..\kst\cmake -G"MinGW Makefiles" -Dkst_release=1 -Dkst_merge_files=1 -Dkst_3rdparty_download=1 -Dkst_install_prefix=%1

mingw32-make
mingw32-make package

