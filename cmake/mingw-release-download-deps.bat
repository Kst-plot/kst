
rmdir /s/q mingw-release-download-deps
mkdir      mingw-release-download-deps
cd         mingw-release-download-deps

::cmake ..\kst\cmake -G"CodeBlocks - MinGW Makefiles" -Dkst_release=1 -Dkst_merge_files=1 -Dkst_3rdparty_download=1 -Dkst_install_prefix=Kst-%1 -Dkst_version_string=%1
cmake ..\kst\cmake -GNinja -Dkst_release=1 -Dkst_merge_files=1 -Dkst_3rdparty_download=1 -Dkst_install_prefix=Kst-%1 -Dkst_version_string=%1

ninja 
ninja install/strip
ninja package
ninja package_source

cd ..

