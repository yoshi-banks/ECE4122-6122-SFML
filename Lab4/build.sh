#!/bin/bash
# Build script for Lab2
# Only works for linux
mkdir -p ../build
cd ../build
cmake ..
make -j$(nproc)