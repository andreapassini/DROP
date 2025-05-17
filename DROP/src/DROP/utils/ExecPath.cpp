#include "ExecPath.h"

#if defined(_WIN32)
#include <windows.h>
#include <Shlwapi.h>
#include <io.h> 

#define access _access_s
#endif

#ifdef __APPLE__
#include <libgen.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#endif

#ifdef __linux__
#include <limits.h>
#include <libgen.h>
#include <unistd.h>

#if defined(__sun)
#define PROC_SELF_EXE "/proc/self/path/a.out"
#else
#define PROC_SELF_EXE "/proc/self/exe"
#endif

#endif


#if defined(_WIN32)

std::string GetExecutablePath()
{
    char rawPathName[MAX_PATH];
    GetModuleFileNameA(NULL, rawPathName, MAX_PATH);
    return std::string(rawPathName);
}

std::string GetExecutableDir()
{
    std::string executablePath = GetExecutablePath();
    char* exePath = new char[executablePath.length()];
    strcpy(exePath, executablePath.c_str());
    PathRemoveFileSpecA(exePath);
    std::string directory = std::string(exePath);
    delete[] exePath;
    return directory;
}

std::string MergePaths(std::string pathA, std::string pathB)
{
    char combined[MAX_PATH];
    PathCombineA(combined, pathA.c_str(), pathB.c_str());
    std::string mergedPath(combined);
    return mergedPath;
}

//std::string GetRelativePath()
//{
//    std::string executableDir = GetExecutablePath();
//    char* exeDir = new char[executableDir.length()];
//    strcpy(exeDir, executableDir.c_str());
//    
//
//
//    std::string relativePath = std::string(exeDir);
//    return relativePath;
//}

#endif

#ifdef __linux__

std::string GetExecutablePath()
{
    char rawPathName[PATH_MAX];
    realpath(PROC_SELF_EXE, rawPathName);
    return  std::string(rawPathName);
}

std::string GetExecutableDir()
{
    std::string executablePath = GetExecutablePath();
    char* executablePathStr = new char[executablePath.length() + 1];
    strcpy(executablePathStr, executablePath.c_str());
    char* executableDir = dirname(executablePathStr);
    delete[] executablePathStr;
    return std::string(executableDir);
}

std::string MergePaths(std::string pathA, std::string pathB)
{
    return pathA + "/" + pathB;
}

#endif

#ifdef __APPLE__
std::string GetExecutablePath()
{
    char rawPathName[PATH_MAX];
    char realPathName[PATH_MAX];
    uint32_t rawPathSize = (uint32_t)sizeof(rawPathName);

    if (!_NSGetExecutablePath(rawPathName, &rawPathSize))
    {
        realpath(rawPathName, realPathName);
    }
    return  std::string(realPathName);
}

std::string GetExecutableDir()
{
    std::string executablePath = GetExecutablePath();
    char* executablePathStr = new char[executablePath.length() + 1];
    strcpy(executablePathStr, executablePath.c_str());
    char* executableDir = dirname(executablePathStr);
    delete[] executablePathStr;
    return std::string(executableDir);
}

std::string MergePaths(std::string pathA, std::string pathB)
{
    return pathA + "/" + pathB;
}
#endif


bool CheckIfFileExists(const std::string& filePath) {
    return access(filePath.c_str(), 0) == 0;
}

std::string GetFullPath(const std::string& relativeFilePath) {
    std::string execDir = GetExecutableDir();
    while (!CheckIfFileExists(execDir + relativeFilePath)) {
        execDir = execDir.substr(0, execDir.find_last_of("\\/"));
    }
    std::string fullPath = execDir + relativeFilePath;
    return fullPath;
}

std::string GetRelativeProjectPath() {
    std::string execDir = GetExecutableDir();
    for (int32_t i = 0; i < 3; i++) {
        execDir = execDir.substr(0, execDir.find_last_of("\\/"));
    }
    return execDir;
}

std::string GetRelativeProjectPathWithMarker(std::string markerFilePath /*= "ProjRelativePathMarker.txt"*/) {
    std::string execDir = GetExecutableDir();
    while (!CheckIfFileExists(execDir + markerFilePath)) {
        execDir = execDir.substr(0, execDir.find_last_of("\\/"));
    }
    return execDir;
}