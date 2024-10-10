#!/usr/bin/env bash
cd /spot-cpp-sdk/cpp
if [ -d build ]; then
  rm -r build
fi
mkdir build
cd build
cmake .. -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=TRUE -DBUILD_CHOREOGRAPHY_LIBS=ON -DBUILD_SHARED_LIBS=ON
make -j8 install package
