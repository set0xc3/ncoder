#!/bin/sh

ROOT=$PWD
NAME=
PROJECT_DIR=$ROOT/code/custom
BUILD_DIR=$ROOT/build

pushd $ROOT/code
  sh ./bin/build-linux.sh
popd

pushd $PROJECT_DIR
  sh ./bin/buildsuper_x64-linux.sh ./ncoder_layer.cpp debug
  mv -f ./custom_4coder.so $BUILD_DIR/custom_4coder.so
popd