@echo off

REM https://gist.github.com/codingminecraft/d5f3d01b33498a2c77014e263ed06eb8

pushd ..
IF "%~1" == "" GOTO GenSlnCleanAll

dependencies\premake-5.0.0-beta2-windows\premake5.exe %1
GOTO Done

:GenSlnCleanAll
dependencies\premake-5.0.0-beta2-windows\premake5.exe clean
dependencies\premake-5.0.0-beta2-windows\premake5.exe vs2022
GOTO Done

:Done

popd
pause