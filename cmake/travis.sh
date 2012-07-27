#!/bin/sh
#
#set -x
#

startdir=$PWD


checkExitCode() {
    exitcode=$?
    if [ $exitcode -ne 0 ]; then
        echo Command failed
        exit 1
    fi
}


#
# print some info about the system
#
echo ------ System
uname -a
lsb_release -a
processors=`grep -c processor /proc/cpuinfo`
echo number of processors: $processors
dpkg --get-selections | grep mingw
iam=`whoami`
if [ "$iam" = "vagrant" ]; then
    sudo rm -rf /usr/lib/jvm
    df -h
fi
echo ------ 


#
# make build directory
#
cd ..
build=_b
if [ -d "$build" ]; then
    echo Removing old build directory $build
    rm -rf $build
fi
mkdir $build
builddir=$PWD/$build
cd $builddir


#
# select mingw version
#
if [ "$1" = "w64" ]; then
    mingw=i686-w64-mingw32
else
    mingw=i586-mingw32msvc
fi


#
# download and install Qt
#
if [ ! -d /usr/local/Trolltech/Qt-win32-g++-$mingw ]; then
    #qttar=Qt-win32-g++-$mingw-4.6.3-dlls-Ubuntu-12.04.tar
    qttar=Qt-win32-g++-$mingw-4.6.1-Ubuntu-11.10.tar
    qt=$builddir/$qttar
    wget https://github.com/downloads/syntheticpp/kst/$qttar.xz
    checkExitCode
    xz -d $qttar.xz
    cd /
    sudo tar xf $qt
    checkExitCode
    echo Checking Qt installation ...
    /usr/local/Trolltech/Qt-win32-g++-$mingw/bin/qmake -query
    checkExitCode
fi


#
# build Kst
#
date=`date --utc '+%Y.%m.%d-%H.%M'`
ver=2.0.6
installed=Kst-$ver-$date
cd $builddir
cmake ../kst/cmake/ -Dkst_release=1 -Dkst_version_string=$ver-$date -Dkst_cross=$mingw -Dkst_install_prefix=./$installed
checkExitCode

make -j $processors
checkExitCode


#
# deploy
#
make package
checkExitCode

if [ ! -e $installed-win32.zip ]; then
    exit 1
fi

if [ "$iam" = "vagrant" ]; then
    cd ~
    tar xf $startdir/cmake/kstdeploy.tar.gz
    checkExitCode
    cd $builddir
fi


if [ "$iam" = "vagrant" ]; then
    git config --global user.name "vargant"
    git config --global user.email vargant@ci.org
fi
git clone --quiet git@github.com:syntheticpp/kstbinary.git
cd kstbinary
git reset --hard HEAD^
cp -f ../$installed-win32.zip .
git add $installed-win32.zip
checkExitCode

git commit --quiet -m"Update win32 binary to version Kst-$ver-$date"
checkExitCode

git push --quiet -f
checkExitCode

cd $startdir

