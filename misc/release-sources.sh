#!/bin/bash

# Usage: release-sources.sh VERSION KDE_SVN_USERNAME
# e.g.: release-sources.sh 2.0.0 username

if [ -z ${1} ]
then
  echo Error: No version number given.
  echo Usage: release-sources.sh VERSION [KDE_SVN_WWW_USERNAME]
  exit 1
fi

KSTDIR=kst-${1}
KSTGZ=$KSTDIR.tar.gz

mkdir $KSTDIR-release
cd $KSTDIR-release

echo Releasing $KSTDIR

svn export svn://anonsvn.kde.org/home/kde/branches/work/kst/portto4/kst $KSTDIR

echo ${1} > $KSTDIR/VERSION

echo Creating $KSTGZ

tar -cf $KSTDIR.tar $KSTDIR
gzip -9 $KSTDIR.tar



if [ -z ${2} ]
then
  exit
fi

# checkout website
svn co --depth=empty svn+ssh://${2}@svn.kde.org/home/kde/trunk/www/areas/extragear/apps/kst www

#add new sources
cp $KSTGZ www
cd www
svn add $KSTGZ

svn status

echo Commit soucres manually to Kst's www directory