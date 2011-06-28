#!/bin/sh

# $1 = path to kst/cmake
# $2 = version name, for instance 2.0.4-beta2

kst_install_prefix=Kst-$2

cmake $1 -Dkst_merge_files=1 -Dkst_3rdparty=1 -Dkst_dataobjects=1 -Dkst_version_string=$2 -Dkst_release=1 -Dkst_install_prefix=$kst_install_prefix

make -j4

make install/strip

#TODO Why is /build/bin/kst2 added to the dmg?
make package
