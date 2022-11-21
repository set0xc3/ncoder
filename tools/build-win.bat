@echo off

set ROOT=%cd%
set NAME=
set PROJECT_DIR=%ROOT%\\code\\custom
set BUILD_DIR=%ROOT%\\build

if not exist "%BUILD_DIR%"\ mkdir "%BUILD_DIR%"

pushd %ROOT%\\code
  call .\\bin\\build.bat
popd

pushd %PROJECT_DIR%
  call .\\bin\\buildsuper_x64-win.bat %PROJECT_DIR%\\ncoder_layer.cpp debug
popd