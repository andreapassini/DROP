#pragma once

#include <string>

namespace File {

void ReadBinaryFile();

void WriteBinaryFile(
	const char* inFilePath, size_t filePathSize
	, void* inBuffer, size_t elementSize, size_t elementCount
);

void WriteBinaryFile(
	std::string inFilePath
	, char* inBuffer, size_t elementSize, size_t elementCount
);

size_t ReadBinaryFile(
	std::string inFilePath
	, void* outBuffer, size_t elementSize, size_t elementCount
);

}