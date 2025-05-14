#!/bin/bash

cmake --build build
cmake --install build
cp build/out/lib/libassistant.so build/out/bin
cd build/out
export LD_LIBRARY_PATH=`pwd`
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH};${LD_LIBRARY_PATH}/lib;${LD_LIBRARY_PATH}/bin"
./bin/kicad