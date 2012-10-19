#!/bin/sh
#
#set -x
#


# ---------------------------------------------------------
#
# set versionname to overwrite generated one
#
versionname=Kst-2.0.7-Beta1



# ---------------------------------------------------------
# 
# helper function to check return code
#
checkExitCode() {
    exitcode=$?
    if [ $exitcode -ne 0 ]; then
        echo Command failed
        exit 1
    fi
}



# ---------------------------------------------------------
#
# print some info about the system
#
echo
echo ----------------------------------- 
echo Build system:
echo
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
echo ----------------------------------- 
echo



# ---------------------------------------------------------
# 
# get sha1 when git is used
#
sha1=`git rev-parse master`
exitcode=$?

if [ $exitcode -eq 0 ]; then
    rev="-Dkst_revision=$sha1"
fi



# ---------------------------------------------------------
#
# make build directory
#
startdir=$PWD
cd ..
build=_b
if [ -d "$build" ]; then
    echo Removing old build directory $build
    rm -rf $build
fi
mkdir $build
builddir=$PWD/$build
cd $builddir



# ---------------------------------------------------------
#
# get actual cmake 
#
cmakever=cmake-2.8.9-Linux-i386
cmakebin=x
if [ ! -d /opt/$cmakever ]; then
    wget http://www.cmake.org/files/v2.8/$cmakever.tar.gz
    checkExitCode
    cd /opt
    sudo tar xf $builddir/cmake-2.8.9-Linux-i386.tar.gz
    checkExitCode
    cd $builddir
fi
cmakebin=/opt/$cmakever/bin/cmake
$cmakebin --version
checkExitCode

mingw=i686-w64-mingw32
if [ "$1" = "qt5" ]; then
    qtver=5.0.0
    gccver=4.7.2
else
    qtver=4.8.4
    gccver=4.7.2
    dw2=-dw2
    extlib=kst-3rdparty-win32-gcc-dw2-4.7.2
    useext="-Dkst_3rdparty=1 -Dkst_3rdparty_dir=/opt/"$extlib
fi
qtver=Qt-$qtver-win32-g++-$mingw$dw2-$gccver
mingwver=i686-w64-mingw32-gcc$dw2-$gccver
mingwdir=mingw32$dw2



# ---------------------------------------------------------
#
# download and install mingw
#
if [ ! -d /opt/$mingwdir ]; then
    mingwtar=$mingwver-release-linux32_rubenvb-Ubuntu32-12.04.tar
    wget https://github.com/downloads/syntheticpp/kst/$mingwtar.xz
    checkExitCode
    xz -d $mingwtar.xz
    cd /opt
    sudo tar xf $builddir/$mingwtar
    checkExitCode
    cd $builddir
fi
# when cross-compiler is in path cmake assumes it is a native compiler and passes "-rdynamic" which mingw doesn't support
#export PATH=/opt/mingw32/bin:$PATH
echo Checking mingw installation ...
/opt/$mingwdir/bin/i686-w64-mingw32-gcc -dumpversion
checkExitCode



# ---------------------------------------------------------
#
# download and install Qt
#
if [ ! -d /opt/$qtver ]; then
    qttar=$qtver-Ubuntu32-12.04.tar
    wget https://github.com/downloads/syntheticpp/kst/$qttar.xz
    checkExitCode
    xz -d $qttar.xz
    cd /opt
    sudo tar xf $builddir/$qttar
    checkExitCode
    cd $builddir
fi
export PATH=/opt/$qtver/bin:$PATH
echo Checking Qt installation ...
which qmake
checkExitCode



# ---------------------------------------------------------
#
# download 3rdparty
#
if [ ! -d /opt/$extlib ]; then
    wget https://github.com/downloads/syntheticpp/kst/$extlib.zip
    checkExitCode
    cd /opt
    sudo unzip -q $builddir/$extlib.zip
    checkExitCode
    cd $builddir
    sudo cp /opt/$extlib/include/matio_pubConf.h /opt/$extlib/include/matio_pubconf.h
fi



# ---------------------------------------------------------
#
# build Kst
#
ver=2.0.x
date=`date --utc '+%Y.%m.%d-%H.%M'`
cd $builddir
if [ "$1" = "qt5" ]; then
    ver=$ver-Qt5
    qtopt="-Dkst_qt5=1 -Dkst_opengl=0"
    versionname=$versionname-Qt5
else
    ver=$ver-Qt4
    qtopt="-Dkst_qt4=/opt/$qtver -Dkst_opengl=0"
fi

if [ -z $versionname ]; then
    versionname=Kst-$ver-$date
fi

$cmakebin ../kst/cmake/ \
    -Dkst_release=1  \
    -Dkst_version_string=$versionname \
    -Dkst_install_prefix=./$versionname \
    -Dkst_cross=/opt/$mingwdir/bin/$mingw \
    $rev $qtopt $useext

checkExitCode

make -j $processors
checkExitCode



# ---------------------------------------------------------
#
# deploy
#
make package
checkExitCode

if [ ! -e $versionname-win32.zip ]; then
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
    else 
        git checkout master
    fi
    git reset --hard HEAD^
    cp -f ../$versionname-win32.zip .
    git add $versionname-win32.zip
    checkExitCode
    
    git commit --quiet -m"Update win32 binary to version $versionname"
    checkExitCode

    git push --quiet -f
    checkExitCode
fi


cd $startdir

