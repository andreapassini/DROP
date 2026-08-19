#if defined(DROP_PLATFORM_WINDOWS) // not sure that putting it here is good, we could simply not include the file in the build

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <cstdio>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "DROP/core/file.h"
#include <string>

void File::ReadBinaryFile() {
	assert(0);
};

void File::WriteBinaryFile(
	const char* inFilePath, size_t filePathSize
	, void* inBuffer, size_t elementSize, size_t elementCount
) {
	FILE* f;
	f = fopen(inFilePath, "w");
	if (!f)
	{
		// no need to close file if error while opening
		DebugBreak();
		return;
	}

	size_t writtenBytes = fwrite(
		inBuffer
		, elementSize
		, elementCount
		, f
	);

	if (writtenBytes == 0)
	{
	DebugBreak();
	}

	fclose(f);
}

void File::WriteBinaryFile(
	std::string inFilePath
	, char* inBuffer, size_t elementSize, size_t elementCount
) {
	FILE* f;
	f = fopen(inFilePath.c_str(), "wb");
	if (!f)
	{
		// no need to close file if error while opening
		DebugBreak();
		return;
	}

	size_t writtenBytes = fwrite(
		inBuffer
		, elementSize
		, elementCount
		, f
	);

	if (writtenBytes == 0)
	{
	DebugBreak();
	}

	fclose(f);
}

size_t File::ReadBinaryFile(
	std::string inFilePath
	, void* outBuffer, size_t elementSize, size_t elementCount
) {
	size_t bytesRead = 0;
	FILE* f;
	f = fopen(inFilePath.c_str(), "rb");
	if (!f)
	{
		DebugBreak();
		return bytesRead;
	}

	fseek(f, 0, SEEK_END);

	uint32_t len = (uint32_t)ftell(f); // in bytes
	if (len < (elementSize * elementCount))
	{
		fclose(f);
		DebugBreak();
		return bytesRead;
	}

	rewind(f); // we moved to the end with fseek SEEK_END

	bytesRead = fread(
		outBuffer
		, elementSize
		, elementCount
		, f
	);

	if (ferror(f))
	{
		char* error;
		perror(error);
		clearerr(f);
		DebugBreak();
	}
	else if (feof(f))
	{     /* possibility 2 */
		DebugBreak();
	}

	if (bytesRead == 0)
	{
		DebugBreak();
	}
	fclose(f);

	return bytesRead;
}

void File::ReadTextFile(
	std::string* inFilePath
	, std::string* outTextContent
) {
	std::ifstream textFile;
	textFile.open(*inFilePath);
	if (!textFile.is_open())
	{
		std::cerr << "Error opening file: " << *inFilePath << std::endl;

		// Check for specific error conditions
		if (textFile.bad())
		{
			std::cerr << "Fatal error: badbit is set." << std::endl;
		}

		if (textFile.fail())
		{
			// Print a more detailed error message using
			// strerror
			std::cerr << "Error details: " << strerror(errno) << std::endl;
		}

		// Handle the error or exit the program
		return;
	}

	std::stringstream textStream;
	textStream << textFile.rdbuf();
	textFile.close();
	
	*outTextContent = textStream.str();
}

void File::WriteTextFile(
	std::string* inFilePath
	, std::string* inTextContent
) {
	if (!inFilePath) return;
	if (!inTextContent) return;

	std::ofstream textFile;
	textFile.open(*inFilePath);
	textFile << inTextContent->c_str();
	textFile.close();
}

FileTime File::GetLastWriteTime(char* filePath)
{
	FileTime lastFileTime = {};

	WIN32_FILE_ATTRIBUTE_DATA Data;
	if (GetFileAttributesEx(filePath, GetFileExInfoStandard, &Data))
	{
		lastFileTime = Data.ftLastWriteTime;
	}

	return lastFileTime;
}

#endif // DROP_PLATFORM_WINDOWS