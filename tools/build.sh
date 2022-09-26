#!/bin/sh

ROOT=$PWD
BIN_NAME=4ed-dev

if [ ! -d "$ROOT/build" ];
then 
  mkdir -p $ROOT/build/
  ln -frs $ROOT/bin/fonts $ROOT/build
  ln -frs $ROOT/bin/themes $ROOT/build
  cp -f $ROOT/bin/4ed $ROOT/build
  cp -f $ROOT/bin/4ed_app.so $ROOT/build
  ln -frs $ROOT/bin/bindings.4coder $ROOT/build
  ln -frs $ROOT/bin/config.4coder $ROOT/build
fi

pidof $BIN_NAME >/dev/null
if [[ $? -ne 0 ]] ; then
  sh $ROOT/code/bin/buildsuper_x64-linux.sh $ROOT/code/ncoder_layer.cpp debug
  mv -f $ROOT/custom_4coder.so $ROOT/build/custom_4coder.so
fi
