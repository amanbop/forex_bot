#!/bin/sh

if [ -f CMakeCache.txt ]
then
  rm CMakeCache.txt
fi

if [ $# -eq 1 ]
then
  cmake CMakeLists.txt -D$1=1 && make clean && make
else
  cmake CMakeLists.txt && make clean && make
fi

if [ $? -eq 0 ]
then
  rm -rf ./bin/
  mkdir -p bin
  mv -f WholeSample ./bin/
  cd bin/
  find ../../../../lib/ -name lib*.* -exec ln -f {} \;
fi  
