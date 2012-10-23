echo off

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

if "%3" == "" (
	set kstdir=..\kst
) else (
	set kstdir=%3
)

if "%4" == "" (
	set download=1
	set build=0
	rmdir /s/q mingw-release-download-deps
	mkdir      mingw-release-download-deps
	cd         mingw-release-download-deps
) else (
	set download=0
	set build=1
)


echo on

cmake %kstdir%\cmake -G"CodeBlocks - MinGW Makefiles" -Dkst_release=1 -Dkst_merge_files=0 -Dkst_3rdparty_download=%download% -Dkst_3rdparty_build=%build%  -Dkst_install_prefix=Kst-%ver% -Dkst_version_string=%ver% -Dkst_make_program=%make%

%make%

mingw32-make install/strip
mingw32-make package
mingw32-make package_source

cd ..
