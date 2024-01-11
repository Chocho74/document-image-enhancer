#!/bin/sh

set -xe

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++ .

make -k && ./enhance 0.5 0.84 original_image.ppm output_image.ppm
