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

if "%3" == "" (
	set kstdir=..\kst
) else (
	set kstdir=%3
)

%kstdir%\cmake\mingw-release-download-deps.bat %ver% %make% %kstdir% build
