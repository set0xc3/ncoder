#!/bin/sh

ROOT=$PWD

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

sh $ROOT/code/bin/buildsuper_x64-linux.sh $ROOT/code/4coder_default_bindings.cpp debug
mv -f $ROOT/custom_4coder.so $ROOT/build/custom_4coder.so