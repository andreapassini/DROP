#pragma once

#include <string>

std::string GetExecutablePath();
std::string GetExecutableDir();
std::string MergePaths(std::string pathA, std::string pathB);
bool CheckIfFileExists(const std::string& filePath);

std::string GetFullPath(const std::string& relativeFilePath);

// get the relative path, 
// it will go 3 steps back from Exec dir.
// If you want to use a marker call GetRelativeProjectPathWithMarker()
std::string GetRelativeProjectPath();

// get the relative path, 
// use the file path a marker.
// Ex: GetRelativeProjectPathWithMarker("\\models\\cube.obj")
std::string GetRelativeProjectPathWithMarker(std::string markerFilePath = "ProjRelativePathMarker.txt");