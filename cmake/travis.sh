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
travis=travis
if [ "$iam" = "$travis" ]; then
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
mingw=i686-w64-mingw32


#
# get actual cmake 
#
if [ "$iam" = "$travis" ]; then
    cd ..
    if [ ! -d $PWD/cmake-2.8.9-Linux-i386 ]; then
        wget http://www.cmake.org/files/v2.8/cmake-2.8.9-Linux-i386.tar.gz
        tar xf cmake-2.8.9-Linux-i386.tar.gz
    fi
    cmakebin=$PWD/cmake-2.8.9-Linux-i386/bin/cmake
    cd $builddir
else
    cmakebin=cmake
fi

#
# download and install Qt
#
if [ "$1" = "qt5" ]; then
    qtver=5.0.0
else
    qtver=4.8.3
fi
qtver=Qt-$qtver-win32-g++-$mingw
    
if [ ! -d /opt/$qtver ]; then
    qttar=$qtver-Ubuntu-11.10.tar
    wget https://github.com/downloads/syntheticpp/kst/$qttar.xz
    checkExitCode
    xz -d $qttar.xz
    cd /
    sudo tar xf $builddir/$qttar
    checkExitCode
    echo Checking Qt installation ...
    readelf -h  /opt/$qtver/bin/qmake
    /opt/$qtver/bin/qmake -query
    checkExitCode
fi

#
# build Kst
#
date=`date --utc '+%Y.%m.%d-%H.%M'`
ver=2.0.6
cd $builddir
if [ "$1" = "qt5" ]; then
    ver=$ver-Qt5
    qtopt="-Dkst_qt5=/opt/$qtver -Dkst_opengl=0"
else
    ver=$ver-Qt4
    qtopt="-Dkst_qt4=/opt/$qtver -Dkst_opengl=0"
fi
installed=Kst-$ver-$date
$cmakebin ../kst/cmake/ -Dkst_release=1 -Dkst_version_string=$ver-$date -Dkst_cross=$mingw -Dkst_install_prefix=./$installed $qtopt
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

if [ "$iam" = "$travis" ]; then
    cd ~
    tar xf $startdir/cmake/kstdeploy.tar.gz
    checkExitCode
    cd $builddir
fi


if [ "$iam" = "$travis" ]; then
    git config --global user.name "travis"
    git config --global user.email travis@noreply.org

    git clone --quiet git@github.com:syntheticpp/kstbinary.git
    cd kstbinary
    if [ "$1" = "qt5" ]; then
        git checkout Qt5
    fi
    git reset --hard HEAD^
    cp -f ../$installed-win32.zip .
    git add $installed-win32.zip
    checkExitCode
    
    git commit --quiet -m"Update win32 binary to version Kst-$ver-$date"
    checkExitCode

    git push --quiet -f
    checkExitCode
fi


cd $startdir

