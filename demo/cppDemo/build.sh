#!/bin/bash

# Very simple shell script to build the C++ demo for X86 or ARM, 32-bit or 64-bit. 
# 
# The demo requires a JPEG libray, which can typically be installed with apt/yum:
#
#     sudo apt-get install libjpeg-dev
#       or
#     sudo yum install libjpeg-turbo-devel
#
# Note this only builds the shared library version (as many Linux distros do not have
# static libraries installed by default).
# 
# Notice    Copyright (C) Cognex Corporation
#
#
ARCH=`uname -m`


if [ "$ARCH" = "arm" ]; then
    LIBDIR=../lib/arm/32bit
elif [ "$ARCH" = "aarch64" ]; then
    LIBDIR=../lib/arm/64bit
elif [ "$ARCH" = "x86_64" ]; then
    LIBDIR=../lib/x86/64bit
else 
    LIBDIR=../lib/x86/32bit
fi

INCDIR=../include
LIBS="-lBarcodeScanner -ljpeg -lpthread"
GCC=g++
GCCOPTS="-g3 -O3 -Wall"

$GCC $GCCOPTS -I"$INCDIR" -c cppDemo.cpp
$GCC $GCCOPTS -I"$INCDIR" -c imageLib.cpp
$GCC $GCCOPTS -I"$INCDIR" -c MWResult.cpp

#g++ -static *.o -L"$LIBDIR" $LIBS -o cppDemo #uncomment for a static build
g++ *.o -L"$LIBDIR" $LIBS -o cppDemoShared -Wl,-rpath,"\$ORIGIN/$LIBDIR"

rm *.o

./cppDemoShared

