#!/bin/sh

echo "-------------------------"
echo "        [Build]          "
echo "-------------------------"

ROOT=$PWD
NAME=
PROJECT_DIR=$ROOT/code/custom
BUILD_DIR=$ROOT/build

if [ ! -d "$BUILD_DIR" ]; then

mkdir "$BUILD_DIR"

fi

pushd $ROOT/code
  ./bin/build-linux.sh
popd

pushd $PROJECT_DIR
  ./bin/buildsuper_x64-linux.sh ./ncoder_layer.cpp debug
  mv -f ./custom_4coder.so $BUILD_DIR/custom_4coder.so
popd
