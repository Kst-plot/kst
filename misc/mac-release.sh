#!/bin/sh

# $1 = path to kst/cmake
# $2 = package name
cmake $1 -Dkst_merge_files=1 -Dkst_3rdparty=1 -Dkst_dataobjects=1 -Dkst_install_prefix=$2 -Dkst_release=1

make -j4

make install/strip

#TODO Why is /build/bin/kst2 added to the dmg?
make package
