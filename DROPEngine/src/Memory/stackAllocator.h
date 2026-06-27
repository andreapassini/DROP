#pragma once

#include "Types/Types.h"
#include "Memory/memoryAllocatorUtils.h"

// from https://www.gingerbill.org/article/2019/02/15/memory-allocation-strategies-003/
// Modified by me

// Extend the Arena Allocator with an header
// - keep track of allocations size -> Realloc

struct StackAllocator
{
	unsigned char* buffer = nullptr;
	size_t bufferLenght = 0LL;
	size_t offset = 0LL;
};

// We need a Fat Header for the REALLOC
// -> Realloc: Resize the allocation and copy the data (beware of the size limit)
// 
// - Size of the allocation to enforce FIFO free
struct StackAllocatorHeader
{
	size_t allocationSize;
};

void StackInit(
	StackAllocator* stackAllocator
	, void* backBuffer
	, size_t backBufferLength
);

// Returning the ptr to allocation after the header
void* StackAlloc(
	StackAllocator* stackAllocator
	, size_t size
	, size_t alignment = DEFAULT_ALIGNMENT
);

void StackFree(
	StackAllocator* stackAllocator
	, void* backBuffer
);

void* StackResize(
	StackAllocator* stackAllocator
	, void* old_memor
	, size_t old_size
	, size_t new_size
	, size_t align = DEFAULT_ALIGNMENT
);

void ArenaFreeAll(
	StackAllocator* stackAllocator
);