@echo off

REM https://gist.github.com/codingminecraft/d5f3d01b33498a2c77014e263ed06eb8

pushd ..
IF "%~1" == "" GOTO PrintHelp
IF "%~1" == "compile" GOTO Compile
IF "%~1" == "compileGame" GOTO CompileGame
IF "%~1" == "genSln" GOTO GenSln
IF "%~1" == "genSlnClean" GOTO GenSlnClean
IF "%~1" == "genSlnCleanAll" GOTO GenSlnCleanAll

dependencies\premake-5.0.0-beta2-windows\premake5.exe %1
GOTO Done

:PrintHelp
echo.
echo Enter 'Setup.bat action' where action is one of the following:
echo.
echo   genSln			Will generate the solution.
echo   genSlnClean		Will clean the prj files and generate the solution.
echo   genSlnCleanAll	Will clean and generate the solution.
echo   compile			Will generate make file then compile using the make file.
echo   compileGame		Will clean pdb and then compile using the make file.
echo   clean			Remove all binaries and intermediate binaries and project files.
echo   codelite			Generate CodeLite project files
echo   gmake			Generate GNU makefiles for Linux
echo   vs2005			Generate Visual Studio 2005 project files
echo   vs2008			Generate Visual Studio 2008 project files
echo   vs2010			Generate Visual Studio 2010 project files
echo   vs2012			Generate Visual Studio 2012 project files
echo   vs2013			Generate Visual Studio 2013 project files
echo   vs2015			Generate Visual Studio 2015 project files
echo   vs2017			Generate Visual Studio 2017 project files
echo   vs2019			Generate Visual Studio 2019 project files
echo   xcode4			Generate Apple Xcode 4 project files
GOTO Done

:GenSln
dependencies\premake-5.0.0-beta2-windows\premake5.exe vs2022
GOTO Done

:GenSlnClean
dependencies\premake-5.0.0-beta2-windows\premake5.exe cleanPdb
dependencies\premake-5.0.0-beta2-windows\premake5.exe vs2022
GOTO Done

:GenSlnCleanAll
dependencies\premake-5.0.0-beta2-windows\premake5.exe clean
dependencies\premake-5.0.0-beta2-windows\premake5.exe vs2022
GOTO Done

:Compile
dependencies\premake-5.0.0-beta2-windows\premake5.exe cleanPdb

GOTO Done

:CompileGame
dependencies\premake-5.0.0-beta2-windows\premake5.exe cleanPdb
REM msbuild /build DROPGame/DropGame.vcxproj
REM msbuild Drop.sln /t:DropGame /p:Configuration="Debug" /p:Platform="x86" /p:BuildProjectReferences=false
msbuild Drop.sln /t:DropGame
GOTO Done

:Done

popd
pause