@echo off

echo "-------------------------"
echo "        [Run]            "
echo "-------------------------"

set ROOT=%cd%
set PROGRAM_NAME=4ed
set BUILD_PATH=%ROOT%\\build

pushd %BUILD_PATH%
	.\\%PROGRAM_NAME%.exe
popd