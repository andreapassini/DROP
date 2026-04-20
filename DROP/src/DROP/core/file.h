#pragma once

#include <string>

namespace File {

void ReadFile();

void WriteFile(
	const char* inFilePath, size_t filePathSize
	, void* inBuffer, size_t elementSize, size_t elementCount
);

void WriteFile(
	std::string inFilePath
	, char* inBuffer, size_t elementSize, size_t elementCount
);

size_t ReadFile(
	std::string inFilePath
	, void* inBuffer, size_t elementSize, size_t elementCount
);

}