
cmake ..\kst\cmake -G"MinGW Makefiles" -Dkst_release=1 -Dkst_merge_files=1 -Dkst_3rdparty_download=1 -Dkst_install_prefix=Kst-2.0.3-beta1

mingw32-make
mingw32-make package

