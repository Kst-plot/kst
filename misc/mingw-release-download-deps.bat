
cmake ..\kst\cmake -G"MinGW Makefiles" -Dkst_release=1 -Dkst_merge_files=1 -Dkst_3rdparty_download=1

mingw32-make
mingw32-make install/strip
