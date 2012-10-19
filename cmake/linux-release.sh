#!/bin/sh

# Usage
#   linux-release <version string> optional: <existing sourceforge dir>  <sourceforge username> <skip-build>
# e.g:
#   build  only     : linux-release.sh 2.0.3-beta2 
#   build and upload: linux-release.sh 2.0.3-beta2 "Kst\\ 2.0.3" username
#   upload only     : linux-release.sh 2.0.3-beta2 "Kst\\ 2.0.3" username skip
#   
set -x

kst_install_prefix=kst-$1

machine_type=`uname -m`
binarytar=${kst_install_prefix}-binary-${machine_type}.tar


if [ -z ${4} ]
then
	cmake ../kst/cmake -Dkst_release=1 -Dkst_merge_files=1 -Dkst_version_string=$1 -Dkst_install_prefix=$kst_install_prefix -Dkst_3rdparty_build=1 -Dkst_deploy=1
	cd 3rdparty
	make -j8
	cd ..
	make -j8
	make 
	make install/strip
	strip ${kst_install_prefix}/lib
	strip ${kst_install_prefix}/lib/kst2/plugins/*
	
    rm	${binarytar}.gz
    tar cf ${binarytar} ${kst_install_prefix}
    gzip -9 ${binarytar}
    make package_source
fi


if [ ! -z ${3} ]
then
	# spaces in $2 "Kst\\ 2.0.3"
	scp \
	${binarytar}.gz \
	${kst_install_prefix}.tar.gz \
	${3},kst@frs.sourceforge.net:"/home/frs/project/k/ks/kst/${2}"
fi
