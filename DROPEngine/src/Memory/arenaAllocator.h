#pragma once

#include "Types/Types.h"
#include "Memory/memoryAllocatorUtils.h"

// From Ginger Bill
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
// and modified

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

template <typename T>
T* ArenaAlloc(
	ArenaAllocator* arena
	, size_t size
	, size_t align = DEFAULT_ALIGNMENT
) {
	// Align 'currentOffset' forward to the specified alignment
	uintptr_t curr_ptr = (uintptr_t)arena->buffer + (uintptr_t)arena->currentOffset;
	uintptr_t offset = AlignForward(curr_ptr, align);
	offset -= (uintptr_t)arena->buffer; // Change to relative offset

	// Check to see if the backing memory has space left
	if (offset + size <= arena->bufferLenght) {
		void* ptr = &arena->buffer[offset];


		arena->previousOffset = offset;
		arena->currentOffset = offset + size;

		// Zero new memory by default
		memset(ptr, 0, size);

		// placement new to init the vptr
		return new (ptr) T();
	}
	// Return NULL if the arena is out of memory (or handle differently)

	return nullptr;
};


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

void ArenaFreeAll(
	ArenaAllocator* arena
);
