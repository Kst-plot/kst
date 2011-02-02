#!/bin/sh


kst_install_prefix=$1

cmake ../kst/cmake -Dkst_release=1 -Dkst_merge_files=1 -Dkst_install_prefix=$kst_install_prefix

make -j4
make package
make package_source

if [ ! -z ${2} ]
then
	# spaces in $2 "Kst\\ 2.0.3"
	scp ${kst_install_prefix}.sh             syntheticpp,kst@frs.sourceforge.net:"/home/frs/project/k/ks/kst/$2"
	scp ${kst_install_prefix}.tar.gz         syntheticpp,kst@frs.sourceforge.net:"/home/frs/project/k/ks/kst/$2"
	scp ${kst_install_prefix}-sources.tar.gz syntheticpp,kst@frs.sourceforge.net:"/home/frs/project/k/ks/kst/$2"
	scp ${kst_install_prefix}-sources.zip    syntheticpp,kst@frs.sourceforge.net:"/home/frs/project/k/ks/kst/$2"
fi

