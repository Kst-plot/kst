echo off 

rmdir /s/q mingw-release-build-deps
mkdir      mingw-release-build-deps
cd         mingw-release-build-deps

if "%1" == "" (
	set ver=2.0.x
) else (
	set ver=%1
)

if "%2" == "" (
	set make=mingw32-make
) else (
	set make=%2
)

echo cmake ..\kst\cmake -G"CodeBlocks - MinGW Makefiles" -Dkst_release=1 -Dkst_merge_files=1 -Dkst_3rdparty_build=1 -Dkst_install_prefix=Kst-%ver% -Dkst_version_string=%ver% -Dkst_make_program=%make%

cmake ..\kst\cmake -G"CodeBlocks - MinGW Makefiles" -Dkst_release=1 -Dkst_merge_files=1 -Dkst_3rdparty_build=1 -Dkst_install_prefix=Kst-%ver% -Dkst_version_string=%ver% -Dkst_make_program=%make%

%make%
%make% install/strip
%make% package
%make% package_source

cd ..
