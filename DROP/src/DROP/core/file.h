#pragma once

namespace File {

void ReadFile();

void WriteFile(
	char* inFilePath, size_t filePathSize
	, void* inBuffer, size_t elementSize, size_t elementCount
);

}