#!/bin/sh
#
#set -x
#
startdir=$PWD


# ---------------------------------------------------------
#
# set 'versionname' to overwrite generated one based on 'ver'
#

#versionname=Kst-2.0.8-rc2

ver=2.0.x
date=`date --utc '+%Y.%m.%d-%H.%M'`
if [ -z $versionname ]; then
    buildinstaller=0
    versionname=Kst-$ver-$date
else
    buildinstaller=1
fi

#if [ "$1" = "qt5" ]; then
#    versionname=$versionname-Qt5
#fi

dep=$HOME/dep
mkdir -p $dep


echo ---------------------------------------------------------
echo ---------- Building $versionname
echo ---------------------------------------------------------


if [ -f "/usr/bin/ninja" ] || [ -f "/usr/local/bin/ninja" ]; then
    generator=-GNinja
    buildcmd=ninja
    buildcmd_parallel=ninja
    export NINJA_STATUS="[%f/%t %o/s, %es] "
else
    buildcmd=make
    buildcmd_parallel="make -j2"
fi

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
echo ---------------------------------------------------------
echo

if [ "$iam" = "$travis" ]; then
    deploybinary=1
else
    deploybinary=0
fi

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
if [ $deploybinary -eq 1 ]; then
    if [ "$iam" = "$travis" ]; then
        cd ~
        tar xf $startdir/cmake/kstdeploy.tar.gz
        checkExitCode
    fi

    cd $startdir
    kstbinary=kst-build
    rm -rf $kstbinary
    mkdir $kstbinary
    cd $kstbinary
    git init --quiet
    git config user.name "travis"
    git config user.email travis@noreply.org
    git remote add origin git@github.com:Kst-plot/$kstbinary.git
    git fetch origin master --quiet
    checkExitCode
fi
cd $startdir


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
if [ "$2" = "x64" ]; then
    build=_b_x64
else
    build=_b_x86
fi
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
cmakever=cmake-3.3.1-Linux-x86_64

if [ "$iam" = "$travis" ]; then
    if [ ! -d $dep/$cmakever ]; then
          cmakebin=x
          if [ ! -d $dep/$cmakever ]; then
              wget http://www.cmake.org/files/v3.3/$cmakever.tar.gz
              checkExitCode
              cd $dep
              tar xf $builddir/$cmakever.tar.gz
              checkExitCode
              cd $builddir
          fi
    fi
    cmakebin=$dep/$cmakever/bin/cmake
else
    cmakebin=cmake
fi
$cmakebin --version
checkExitCode

gccver=4.7.2
if [ "$2" = "x64" ]; then
    win=win64
    mingw=x86_64-w64-mingw32
    exc=-seh
    mingwdir=mingw64$exc
    branch=Kst-64bit-no-3rdparty-plugins-Qt5
    extlib=
    useext=
else
    win=win32
    mingw=i686-w64-mingw32
    exc=-dw2
    mingwdir=mingw32$exc
    branch=Kst-32bit-3rdparty-plugins-Qt5
    extlib=kst-3rdparty-win32-gcc$exc-4.7.2
    useext="-Dkst_3rdparty=1 -Dkst_3rdparty_dir=$dep/"$extlib
fi

if [ "$1" = "qt5" ]; then
    qtver=5.5.0
    tarver=
else
    qtver=4.8.4
    tarver=
    branch=Kst-32bit-3rdparty-plugins-Qt4
fi

qtver=Qt-$qtver-$win-g++-$mingw$exc-$gccver
mingwver=$mingw-gcc$exc-$gccver


server=http://sourceforge.net/projects/kst/files/3rdparty
# ---------------------------------------------------------
#
# download and install mingw
#
if [ ! -d $dep/$mingwdir ]; then
    mingwtar=$mingwver-Ubuntu64-12.04.tar
    wget $server/$mingwtar.xz
    checkExitCode
    xz -d $mingwtar.xz
    cd $dep
    tar xf $builddir/$mingwtar
    checkExitCode
    cd $builddir
fi
# when cross-compiler is in path cmake assumes it is a native compiler and passes "-rdynamic" which mingw doesn't support
#export PATH=$dep/mingw32/bin:$PATH
echo Checking mingw installation ...
$dep/$mingwdir/bin/$mingw-gcc -dumpversion
checkExitCode



# ---------------------------------------------------------
#
# download and install Qt
#
if [ ! -d $dep/$qtver ]; then
    qttar=$qtver-Ubuntu64-12.04$tarver.tar
    wget $server/$qttar.xz
    checkExitCode
    xz -d $qttar.xz
    cd $dep
    tar xf $builddir/$qttar
    checkExitCode
    echo -e "[Paths]\nPrefix = $dep/$qtver" > $dep/$qtver/bin/qt.conf
    cd $builddir
fi
export PATH=$dep/$qtver/bin:$PATH
echo Checking Qt installation ...
which qmake
checkExitCode



# ---------------------------------------------------------
#
# download 3rdparty
#
if [ ! -d $dep/$extlib ]; then
    wget $server/$extlib.zip
    checkExitCode
    cd $dep
    unzip -q $builddir/$extlib.zip
    checkExitCode
    cd $builddir
    cp $dep/$extlib/include/matio_pubConf.h $dep/$extlib/include/matio_pubconf.h
fi


# ---------------------------------------------------------
#
# checkout translations
#
cd $startdir
./get-translations
cd $builddir


# ---------------------------------------------------------
#
# build Kst
#
cd $builddir
if [ "$1" = "qt5" ]; then
    qtopt="-Dkst_qt5=1"
else
    qtopt="-Dkst_qt4=$dep/$qtver"
fi


if [ $buildinstaller -eq 1 ]; then
    console=-Dkst_console=0
else
    console=-Dkst_console=1
    noinstaller=-Dkst_noinstaller=1
fi

$cmakebin ../kst \
    -Dkst_release=1  \
    -Dkst_version_string=$versionname \
    -Dkst_install_prefix=./$versionname \
    -Dkst_cross=$dep/$mingwdir/bin/$mingw \
    $rev $qtopt $useext $console $noinstaller $generator

checkExitCode


$buildcmd_parallel
checkExitCode


# ---------------------------------------------------------
#
# deploy
#
$buildcmd package
checkExitCode


deploy() {
    if [ ! -e $1 ]; then
        exit 1
    fi
    cd $startdir/$kstbinary
    git checkout master
    git branch -D $2
    git checkout -b $2
    cp -f $builddir/$1 .
    checkExitCode
    git add $1
    checkExitCode

    git commit --quiet -m"Update $win binary to version $versionname"
    checkExitCode

    git push --quiet origin HEAD -f
    checkExitCode
}


if [ $deploybinary -eq 1 ]; then
    if [ $buildinstaller -eq 1 ]; then
        deploy $versionname-$win.exe Installer-$branch
    else
        deploy $versionname-$win.zip $branch
    fi
fi


cd $startdir

