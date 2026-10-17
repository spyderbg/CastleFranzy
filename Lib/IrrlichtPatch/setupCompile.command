#!/bin/sh
         
source=`dirname $0`
target=$source/../Irrlicht

cp $source/include/*.h $target/include/

cp $source/source/Irrlicht/*.cpp $target/source/Irrlicht/
cp $source/source/Irrlicht/*.vcproj $target/source/Irrlicht/

# Collada converter tool
cp $source/tools/ColladaConverter/*.cpp $target/tools/ColladaConverter/src/
cp $source/tools/ColladaConverter/*.h $target/tools/ColladaConverter/src/
cp $source/tools/ColladaConverter/*.exe $target/tools/ColladaConverter/bin/Release/

