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
	size_t bufferLength = 0LL;
	size_t offset = 0LL;
};

// We need a Fat Header for the REALLOC
// -> Realloc: Resize the allocation and copy the data (beware of the size limit)
// 
// - Size of the allocation to enforce FIFO free
struct StackAllocatorHeader
{
	// stackAllocator->offset + padding + size
	size_t padding = 0LL; // maybe this can be calculated
	size_t allocationSize = 0LL;
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

template <typename T>
T* StackAlloc(
	StackAllocator* stackAllocator
	, size_t size
	, size_t alignment = DEFAULT_ALIGNMENT
) {
	uintptr_t curr_addr, next_addr;
	size_t padding;
	StackAllocatorHeader* header;

	assert(IsPowerOfTwo(alignment));

	curr_addr = (uintptr_t)stackAllocator->buffer + (uintptr_t)stackAllocator->offset;

	padding = CalulatePaddingWithHeader(
		curr_addr
		, (uintptr_t)alignment
		, sizeof(StackAllocatorHeader)
	);

	if (stackAllocator->offset + padding + size > stackAllocator->bufferLength)
	{
		// Stack allocator is out of memory
		return nullptr;
	}
	stackAllocator->offset += padding;

	next_addr = curr_addr + (uintptr_t)padding;
	header = (StackAllocatorHeader*)(next_addr - sizeof(StackAllocatorHeader));

	header->padding = padding;
	header->allocationSize = size;

	stackAllocator->offset += size;

	void* ptr = (void*)(next_addr);
	memset(ptr, 0, size);

	return new (ptr) T();
};


void StackFree(
	StackAllocator* stackAllocator
	, void* ptr
);

void* StackResize(
	StackAllocator* stackAllocator
	, void* ptr
	, void* old_memory
	, size_t oldSize
	, size_t newSize
	, size_t align = DEFAULT_ALIGNMENT
);

void* StackRealloc(
	StackAllocator* stackAllocator
	, void* ptr
	, size_t newSize
	, size_t alignment = DEFAULT_ALIGNMENT
);

void StackFreeAll(
	StackAllocator* stackAllocator
);

