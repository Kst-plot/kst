#!/bin/sh

# Usage
#   linux-release <version string> optional: <existing sourceforge dir>  <sourceforge username> <skip-build>
# e.g:
#   build  only     : linux-release.sh 2.0.3-beta2 
#   build and upload: linux-release.sh 2.0.3-beta2 "Kst\\ 2.0.3" username
#   upload only     : linux-release.sh 2.0.3-beta2 "Kst\\ 2.0.3" username skip
#   

kst_install_prefix=kst-$1

if [ -z ${4} ]
then
	cmake ../kst/cmake -Dkst_release=1 -Dkst_merge_files=1 -Dkst_version_string=$1 -Dkst_install_prefix=$kst_install_prefix
	make -j4
	make install
	make package
	make package_source
fi


if [ ! -z ${3} ]
then
	# spaces in $2 "Kst\\ 2.0.3"
	scp \
	${kst_install_prefix}-binary.sh \
	${kst_install_prefix}.tar.gz \
	${kst_install_prefix}.tar.bz2 \
	${3},kst@frs.sourceforge.net:"/home/frs/project/k/ks/kst/${2}"
fi
