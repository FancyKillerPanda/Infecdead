#!/bin/bash

originalDir=$PWD
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd )"
projectRoot=$scriptDir/..

cd $projectRoot/bin 2>/dev/null && rm ./Infecdead

mkdir $projectRoot/build >/dev/null 2>/dev/null
cd $projectRoot/build

cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
cmake --build .

echo
cd $projectRoot/bin
./Infecdead

cd $originalDir
