#pragma once

#include "Types/Types.h"

// From Ginger Bill
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
// and modified

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

struct ArenaAllocator
{
	unsigned char* buffer = nullptr;
	size_t bufferLenght = 0LL;
	size_t previousOffset = 0LL; // useful for resize, avoid reallocating if the resize is on the last chuck
	size_t currentOffset = 0LL;
};

void ArenaInit(
	ArenaAllocator* arena
	, void* backingBuffer
	, size_t backingBufferLength
);

void* ArenaAlloc(
	ArenaAllocator* arena
	, size_t size
	, size_t align = DEFAULT_ALIGNMENT
);

void ArenaFree(
	ArenaAllocator* arena
	, void* ptr
);

void* ArenaResize(
	ArenaAllocator* arena
	, void* old_memor
	, size_t old_size
	, size_t new_size
	, size_t align = DEFAULT_ALIGNMENT
);


// Utils
bool IsPowerOfTwo(
	uintptr_t x
);
uintptr_t AlignForward(
	uintptr_t ptr
	, size_t align
);