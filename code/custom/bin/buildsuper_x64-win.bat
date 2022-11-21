@echo off

:: usage: <script> [target [mode]]
::  works from any directory, but must ::ain in custom/bin path to work
::  target : if specified determines the file that acts as the build target
::           when not specified the build target is 4coder_default_bindings.cpp
::  mode : if set to "release" builds with optimizations
::  The build steps are:
::   1. preprocess the build target
::   2. build the metadata generator
::   3. run the metadata generator on the result from (1)
::   4. build the build target
::   5. cleanup after the metadata generator
::  All output files are generated in the current directory when the script is run

set location=%cd%
set me=%~dp0
cd %me%
cd ..
set custom_root=%cd%
set custom_bin=%custom_root%\bin
cd ..\..\build
set build_root=%cd%
cd %location%

if NOT "%Platform%" == "X64" IF NOT "%Platform%" == "x64" (call "%custom_bin%\setup_cl_x64.bat")

set target=%1
if "%target%" == "" set target=%custom_root%\4coder_default_bindings.cpp

set debug=/Zi
set release=/O2 /Zi
set mode=%debug%
if "%2" == "release" (set mode=%release%)

set binname=%3
if "%binname%" == "" set binname="custom_4coder"

set opts=/W4 /wd4310 /wd4100 /wd4201 /wd4505 /wd4996 /wd4127 /wd4510 /wd4512 /wd4610 /wd4457 /WX
set opts=%opts% /GR- /nologo /FC
set opts=%opts% -I"%custom_root%"
set opts=%opts% /D OS_WINDOWS=1 /D OS_LINUX=0 /D OS_MAC=0
set opts=%opts% %mode%

set preproc_file=4coder_command_metadata.i
set meta_opts=/P /Fi"%preproc_file%" /DMETA_PASS

set build_dll=/LD /link /OPT:REF /RELEASE /PDBALTPATH:%%%%_PDB%%%%
set build_dll=%build_dll% /EXPORT:get_version /EXPORT:init_apis

pushd %build_root%
  call cl %opts% %meta_opts% "%target%"
  call cl %opts% "%custom_root%\4coder_metadata_generator.cpp" /Femetadata_generator
  metadata_generator -R "%custom_root%" "%cd%\%preproc_file%"
  call cl %opts% "%target%" /Fe%binname% %build_dll%
popd