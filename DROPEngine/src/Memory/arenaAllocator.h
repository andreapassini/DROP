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

#ifdef DEBUG_ARENA_ALLOCATOR
		uintptr_t currentBuffer = (uintptr_t)arena->buffer;
		uintptr_t currentOffset = (uintptr_t)arena->currentOffset;
		uintptr_t ptrInUInt = (uintptr_t)ptr;

		//LOG_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);
		LOG_CORE_INFO("Print before memset:\n \
			arena->buffer={0}\n \
			\tarena->bufferLenght = {1}\n \
			\tarena->currentOffset = {2}\n \
			\tsize = {3}\n \
			\tptr = {4}\n \
			\toffset = {5}\n \
			\tptr + size= {6}\n \
			\tcurr_ptr = {7}"
			, (uintptr_t)arena->buffer, arena->bufferLenght, (uintptr_t)arena->currentOffset
			, size, (uintptr_t)ptr, offset, (uintptr_t)((uintptr_t)ptr + (uintptr_t)size), (uintptr_t)curr_ptr
		);
#endif // DEBUG_ARENA_ALLOCATOR

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


// Utils
bool IsPowerOfTwo(
	uintptr_t x
);
uintptr_t AlignForward(
	uintptr_t ptr
	, size_t align
);