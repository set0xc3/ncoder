#!/bin/sh

ROOT=$PWD
NAME=4ed-dev
BUILD=$ROOT/build

if [ -d $BUILD ]; then
  cd $BUILD

  pidof $NAME >/dev/null
  if [[ $? -ne 0 ]] ; then
    $BUILD/$NAME
  fi
fi
