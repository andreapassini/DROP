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
	, void* inBuffer, size_t elementSize, size_t elementCount
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

#endif // DROP_PLATFORM_WINDOWS