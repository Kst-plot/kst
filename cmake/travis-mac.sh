#!/bin/bash

# Unofficial bash strict mode -- http://redsymbol.net/articles/unofficial-bash-strict-mode/
set -euo pipefail
IFS=$'\n\t'


# Install dependencies
brew cask uninstall oclint || true # conflicts with gcc
brew install qt gsl netcdf cfitsio libgetdata
export PATH="/usr/local/opt/qt/bin:$PATH"


# Build kst
./get-translations

mkdir -p build
cd build
rm -fr ./build  # to allow us to run this script multiple times
cmake -Dkst_qt5=1 -Dkst_svnversion=1  -Dkst_dataobjects=1 -Dkst_3rdparty=1 -Dkst_release=1 ..
make


# Add Qt to kst2.app, and replace references to the Qt outside of kst2.app with references
# to Qt inside kst2.app.
macdeployqt ./build/bin/kst2.app

lib_folders=(./build/bin/kst2.app/Contents/plugins/ ./build/bin/kst2.app/Contents/Frameworks/)
to_replace=()

# Copy external libraries to Frameworks and add the names of external libraries to to_replace.
# We don't do this for Qt libraries, because macdeployqt already did that for us.
for folder in "${lib_folders[@]}"; do
	to_copy=($(find -E $folder -regex '.*\.(dylib|so)' -type f -exec otool -L {} \; | sort | uniq | grep '^\t/usr/local' | sed -e $'s/^\t//g' | sed -e $'s/ .*//g'))
	echo "${to_copy[@]}" | xargs -J % cp -f % ./build/bin/kst2.app/Contents/Frameworks
	to_replace+=("${to_copy[@]}")
	chmod 755 $folder/*
done


# Remove references to external libraries.
# We're really stupid here and just try to replace all external references in everything in
# Frameworks and plugins.
# TODO(joshua): Use otool -L to figure out what to replace
for orig_lib in "${to_replace[@]}"; do
	new_lib=$(echo $orig_lib | sed -e 's,.*/,@rpath/,g')
	echo "Running install_name_tool on libraries to replace $orig_lib with $new_lib"
	for folder in "${lib_folders[@]}"; do
		files=($(find -E $folder -regex '.*\.(dylib|so)' -type f))
		for file in "${files[@]}"; do
			install_name_tool -change $orig_lib $new_lib $file
		done
	done
done


# Zip it!
date=$(date -u +"%Y.%m.%d-%H.%M")
bundle_name="kst-plot-$date-macos.zip"
pushd ./build/bin
zip -r "../../$bundle_name" ./kst2.app
popd


# Deploy it
cp ../cmake/kstdeploy.tar.gz ~
pushd ~
tar -xf kstdeploy.tar.gz
popd

kstbinary=kst-build
rm -rf $kstbinary
mkdir $kstbinary
cd $kstbinary
git init --quiet
git config user.name "travis"
git config user.email travis@noreply.org
git remote add origin git@github.com:Kst-plot/$kstbinary.git
git fetch origin master --quiet
git checkout master
git branch -D macos || true
git checkout -b macos
cp -f ../$bundle_name .
git add $bundle_name
git commit --quiet -m "Update Mac binary"
git push --quiet origin HEAD -f
