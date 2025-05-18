#ifdef DROP_PLATFORM_LINUX

#include <limits.h>
#include <libgen.h>
#include <unistd.h>

#if defined(__sun)
#define PROC_SELF_EXE "/proc/self/path/a.out"
#else
#define PROC_SELF_EXE "/proc/self/exe"
#endif

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