#!/bin/sh
echo Copying files...
cp -rf ../build/build/bin/kst2 ./kst/kst 
cp -rf ../build/build/lib/* ./kst/lib/
cd ./kst/lib
echo Stripping libraries...
find -L . -name "*.so*" | xargs arm-unknown-nto-qnx6.5.0-strip
cd ../../
echo Done. To deploy this onto your device, please read README.QNX
