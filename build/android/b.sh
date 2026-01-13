#!/bin/sh

#
ndk-build
# clean
find ./obj/ -name "*.so" | xargs rm -f
find ./obj/ -name "objs" | xargs rm -r
cp ../../src/*.h ./obj/local/

#for release
rm -rf ./ezThread/
mv ./obj/local ./ezThread/

