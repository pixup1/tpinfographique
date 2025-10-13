#!/bin/bash

cd "$( dirname "${BASH_SOURCE[0]}" )"
pwd

cd sfmlGraphicsPipeline/build
cmake ../
make -j6

cd ../../sampleProject/build
../run.sh "$1" ccr