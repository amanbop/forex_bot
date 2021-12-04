#!/bin/sh

if [ -f CMakeCache.txt ]
then
  rm CMakeCache.txt
fi
cmake CMakeLists.txt && make clean && make

if [ $? -eq 0 ]
then
  rm -rf ./bin/
  mkdir -p bin
  mv -f ForexConnect_Sample_Linux ./bin/
  cd bin/
  find ../lib -name lib*.* -exec ln -f {} \;
fi  
