#!/bin/sh
#
#set -x
#
startdir=$PWD


# ---------------------------------------------------------
#
# set 'versionname' to overwrite generated one based on 'ver'
#

#versionname=Kst-2.0.7-Beta1

ver=2.0.7-Beta
date=`date --utc '+%Y.%m.%d-%H.%M'`
if [ -z $versionname ]; then
    versionname=Kst-$ver-$date
fi
if [ "$1" = "qt5" ]; then
    versionname=$versionname-Qt5
fi
echo ---------------------------------------------------------
echo ---------- Building $versionname
echo ---------------------------------------------------------



# ---------------------------------------------------------
#
# print some info about the system
#
echo
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
echo ---------------------------------------------------------
echo



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
# checkout kstbinary
#
if [ "$iam" = "$travis" ]; then
    cd ~
    tar xf $startdir/cmake/kstdeploy.tar.gz
    checkExitCode
    
    cd $startdir
    git config --global user.name "travis"
    git config --global user.email travis@noreply.org
    git clone --quiet git@github.com:syntheticpp/kstbinary.git
    exitcode=$?
    if [ $exitcode -ne 0 ]; then
        rm -rf kstbinary
        git clone --quiet git@github.com:syntheticpp/kstbinary.git
    fi
    checkExitCode
fi



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
cmakever=cmake-2.8.10.2-Linux-i386

if [ ! -d /opt/$cmakever ]; then
	cmakebin=x
	if [ ! -d /opt/$cmakever ]; then
		wget http://www.cmake.org/files/v2.8/$cmakever.tar.gz
		checkExitCode
		cd /opt
		sudo tar xf $builddir/$cmakever.tar.gz
		checkExitCode
		cd $builddir
	fi
fi
cmakebin=/opt/$cmakever/bin/cmake
$cmakebin --version
checkExitCode

mingw=x86_64-w64-mingw32
if [ "$1" = "qt5" ]; then
    qtver=5.0.1
    tarver=
    gccver=4.7.2
    exc=-seh # or -dw2
    extlib=
    useext=
else
    qtver=4.8.4
    tarver=
    gccver=4.7.2
    exc=-dw2
    extlib=kst-3rdparty-win32-gcc-$exc-4.7.2
    useext="-Dkst_3rdparty=1 -Dkst_3rdparty_dir=/opt/"$extlib
fi
qtver=Qt-$qtver-win32-g++-$mingw$exc-$gccver
mingwver=x86_64-w64-mingw32-gcc$exc-$gccver
mingwdir=mingw64$exc


server=http://sourceforge.net/projects/kst/files/3rdparty
# ---------------------------------------------------------
#
# download and install mingw
#
if [ ! -d /opt/$mingwdir ]; then
    mingwtar=$mingwver-Ubuntu64-12.04.tar
    wget $server/$mingwtar.xz
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
/opt/$mingwdir/bin/x86_64-w64-mingw32-gcc -dumpversion
checkExitCode



# ---------------------------------------------------------
#
# download and install Qt
#
if [ ! -d /opt/$qtver ]; then
    qttar=$qtver-Ubuntu64-12.04$tarver.tar
    wget $server/$qttar.xz
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
    wget $server/$extlib.zip
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
cd $builddir
if [ "$1" = "qt5" ]; then
    qtopt="-Dkst_qt5=1 -Dkst_opengl=0"
else
    qtopt="-Dkst_qt4=/opt/$qtver -Dkst_opengl=0"
fi

$cmakebin ../kst/cmake/ \
    -Dkst_release=1  \
    -Dkst_version_string=$versionname \
    -Dkst_install_prefix=./$versionname \
    -Dkst_cross=/opt/$mingwdir/bin/$mingw \
    $rev $qtopt $useext

checkExitCode

processors=4 # /proc reports 32
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
    cd $startdir/kstbinary
    if [ "$1" = "qt5" ]; then
        git checkout Qt5
    else 
        git checkout master
    fi
    git reset --hard HEAD^
    cp -f $builddir/$versionname-win32.zip .
    git add $versionname-win32.zip
    checkExitCode
    
    git commit --quiet -m"Update win32 binary to version $versionname"
    checkExitCode

    git push --quiet -f
    checkExitCode
fi


cd $startdir

