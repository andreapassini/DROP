#ifdef DROP_PLATFORM_APPLE
#include <libgen.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <unistd.h>

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