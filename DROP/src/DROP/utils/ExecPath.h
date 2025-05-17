#pragma once

#include <string>

// Source
// https://stackoverflow.com/questions/1528298/get-path-of-executable

std::string GetExecutablePath();
std::string GetExecutableDir();
std::string MergePaths(std::string pathA, std::string pathB);
bool CheckIfFileExists(const std::string& filePath);

// Get the full path of the file, 
// (descending from exe dir. Beware to add all the relative folders as done in ShaderSetup)
std::string GetFullPath(const std::string& relativeFilePath);

// get the relative path, 
// it will go 3 steps back from Exec dir.
// If you want to use a marker call GetRelativeProjectPathWithMarker()
std::string GetRelativeProjectPath();

// get the relative path, 
// use the file path a marker.
// Ex: GetRelativeProjectPathWithMarker("\\models\\cube.obj")
std::string GetRelativeProjectPathWithMarker(std::string markerFilePath = "ProjRelativePathMarker.txt");