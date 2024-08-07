echo off

if "%1" == "" (
  set ver=2.1
) else (
	set ver=%1
)

if "%2" == "" (
	set make=mingw32-make
    set gen="CodeBlocks - MinGW Makefiles"
) else (
	set make=%2
    set gen=Ninja
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

cmake %kstdir% -G%gen% -Dkst_release=1 -Dkst_merge_files=0 -Dkst_3rdparty_download=%download% -Dkst_3rdparty_build=%build%  -Dkst_install_prefix=Kst-%ver% -Dkst_version_string=%ver% -Dkst_make_program=%make%

%make%

%make% install/strip
%make% package
%make% package_source

cd ..
