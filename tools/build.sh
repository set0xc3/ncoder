#!/bin/sh

ROOT=$PWD

[ ! -d "$ROOT/build" ] && mkdir -p $ROOT/build/

sh $ROOT/code/bin/buildsuper_x64-linux.sh $ROOT/code/4coder_default_bindings.cpp debug
#mv $ROOT/custom_4coder.so $ROOT/build/custom_4coder.so
