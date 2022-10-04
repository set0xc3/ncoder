#!/bin/sh

ROOT=$PWD
NAME=4ed
BUILD=$ROOT/build

pushd $BUILD
  ./$NAME
popd
