#if defined(DROP_PLATFORM_WINDOWS) // not sure that putting it here is good, we could simply not include the file in the build

#include "ExecPath.h"

#include <windows.h>
#include <Shlwapi.h>
#include <io.h> 

// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-s-waccess-s?view=msvc-170
#define access _access_s

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

bool CheckIfFileExists(const std::string& filePath)
{
    return access(filePath.c_str(), 0) == 0;
}

std::string GetFullPath(const std::string& relativeFilePath)
{
    std::string execDir = GetExecutableDir();
    while (!CheckIfFileExists(execDir + relativeFilePath))
    {
        execDir = execDir.substr(0, execDir.find_last_of("\\/"));
    }
    std::string fullPath = execDir + relativeFilePath;
    return fullPath;
}

std::string GetRelativeProjectPath()
{
    std::string execDir = GetExecutableDir();
    for (int32_t i = 0; i < 3; i++)
    {
        execDir = execDir.substr(0, execDir.find_last_of("\\/"));
    }
    return execDir;
}

std::string GetRelativeProjectPathWithMarker(std::string markerFilePath /*= "ProjRelativePathMarker.txt"*/)
{
    std::string execDir = GetExecutableDir();
    while (!CheckIfFileExists(execDir + markerFilePath))
    {
        execDir = execDir.substr(0, execDir.find_last_of("\\/"));
    }
    return execDir;
}

#endif