@echo off

REM https://gist.github.com/codingminecraft/d5f3d01b33498a2c77014e263ed06eb8

pushd ..
IF "%~1" == "" GOTO CompileGame

dependencies\premake-5.0.0-beta2-windows\premake5.exe %1
GOTO Done

:CompileGame
dependencies\premake-5.0.0-beta2-windows\premake5.exe cleanPdb
REM Use Solution folders and dont use back slashes in path
REM https://learn.microsoft.com/it-it/cpp/build/reference/compiler-options-listed-alphabetically?view=msvc-170
REM https://learn.microsoft.com/en-us/visualstudio/msbuild/how-to-build-specific-targets-in-solutions-by-using-msbuild-exe?view=visualstudio&viewFallbackFrom=vs-2019
REM https://learn.microsoft.com/en-us/visualstudio/ide/configure-build-run-options?view=visualstudio
REM -consoleLoggerParameters:PerformanceSummary;NoSummary
REM /consoleLoggerParameters:ErrorsOnly;PerformanceSummary;NoSummary
msbuild Drop.sln /t:Tools\DropGame /p:BuildProjectReferences=false /verbosity:quiet /consoleLoggerParameters:PerformanceSummary;NoSummary
REM for dll error, copy the dll code into temp_dll file
REM Add msbuild to env var PATH (%ProgramFiles%\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin) 
REM and RESTART
GOTO Done

:Done

popd
pause