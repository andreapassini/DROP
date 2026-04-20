#if defined(DROP_PLATFORM_WINDOWS) // not sure that putting it here is good, we could simply not include the file in the build

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <cstdio>
#include <cassert>

#include "DROP/core/file.h"
#include <string>

void File::ReadFile() {
	assert(0);
};

void File::WriteFile(
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

void File::WriteFile(
	std::string inFilePath
	, char* inBuffer, size_t elementSize, size_t elementCount
) {
	FILE* f;
	f = fopen(inFilePath.c_str(), "w");
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

size_t File::ReadFile(
	std::string inFilePath
	, void* inBuffer, size_t elementSize, size_t elementCount
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
		inBuffer
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

#endif // DROP_PLATFORM_WINDOWS