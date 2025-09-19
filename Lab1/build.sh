#!/bin/bash
# Build script for Lab1
# Only works for linux
mkdir -p ../build/Lab1
cd ../build/Lab1
cmake ../../Lab1
make
cd ../../Lab1