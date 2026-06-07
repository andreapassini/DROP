#pragma once

#include <string>

#if defined(DROP_PLATFORM_WINDOWS) // not sure that putting it here is good, we could simply not include the file in the build

using FileTime = FILETIME;

#endif // DROP_PLATFORM_WINDOWS

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

FileTime GetLastWriteTime(
	char* filePath
);

}