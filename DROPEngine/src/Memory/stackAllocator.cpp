#include "Memory/poolAllocator.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "Memory/memoryAllocatorUtils.h"
#include "stackAllocator.h"

void StackInit(
	StackAllocator* stackAllocator
	, void* backBuffer
	, size_t backBufferLength
) {
	stackAllocator->buffer = (unsigned char *)backBuffer;
	stackAllocator->bufferLength = backBufferLength;
	stackAllocator->offset = 0LL;
}

void* StackAlloc(
	StackAllocator* stackAllocator
	, size_t size
	, size_t alignment /*= DEFAULT_ALIGNMENT*/
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

	return memset((void*)next_addr, 0, size);
}


//For stack_free, the pointer passed needs to be checked as to whether it is valid(i.e.it was allocated by this allocator).]
//If it is valid, this means it is possible to acquire the header of this allocation.Using a little pointer arithmetic, 
//we can reset the offset to the allocation previous to the passed pointer.
void StackFree(
	StackAllocator* stackAllocator
	, void* ptr
) {
	if (!ptr)
	{
		return;
	}

	uintptr_t start, end, curr_addr;
	StackAllocatorHeader* header;
	size_t prev_offset;

	start = (uintptr_t)stackAllocator->buffer;
	end = start + (uintptr_t)stackAllocator->bufferLength;
	curr_addr = (uintptr_t)ptr;

	if (!(start <= curr_addr && curr_addr < end))
	{
		assert(0 && "Out of bounds memory address passed to stack allocator (free)");
		return;
	}

	if (curr_addr >= start + (uintptr_t)stackAllocator->offset)
	{
		// Allow double frees
		return;
	}


	header = (StackAllocatorHeader*)(curr_addr - sizeof(StackAllocatorHeader));
	prev_offset = (size_t)(curr_addr - (uintptr_t)header->padding - start);

	// Check for FIFO condition, this should be the last allocation
	if (curr_addr + header->allocationSize == start + stackAllocator->offset)
	{
		stackAllocator->offset = prev_offset;
	}
}

void* StackResize(
	StackAllocator* stackAllocator
	, void* ptr
	, void* old_memory
	, size_t oldSize
	, size_t newSize
	, size_t alignment /*= DEFAULT_ALIGNMENT*/
) {
	if (!ptr)
	{
		return StackAlloc(stackAllocator, newSize, alignment);
	}
	else if (newSize == 0)
	{
		StackFree(stackAllocator, ptr);
		return nullptr;
	}

	uintptr_t start, end, curr_addr;
	size_t min_size = oldSize < newSize ? oldSize : newSize;
	void* new_ptr;

	start = (uintptr_t)stackAllocator->buffer;
	end = start + (uintptr_t)stackAllocator->bufferLength;
	curr_addr = (uintptr_t)ptr;

	if (!(start <= curr_addr && curr_addr < end))
	{
		assert(0 && "Out of bounds memory address passed to stack allocator (resize)");
		return nullptr;
	}

	if (curr_addr >= start + (uintptr_t)stackAllocator->offset)
	{
		// Treat as a double free
		return nullptr;
	}

	if (oldSize == newSize) {
		return ptr;
	}

	new_ptr = StackAlloc(stackAllocator, newSize, alignment);
	memmove(new_ptr, ptr, min_size);
	return new_ptr;
}

void* StackRealloc(
	StackAllocator* stackAllocator
	, void* ptr
	, size_t newSize
	, size_t alignment = DEFAULT_ALIGNMENT
) {
	// ensure allocation in stack allocator
	if (!ptr)
	{
		return StackAlloc(stackAllocator, newSize);
	}
	else if (newSize == 0)
	{
		StackFree(stackAllocator, ptr);
		return nullptr;
	}
	
	// Calculate the old size from the header
	uintptr_t start, end, curr_addr;
	void* new_ptr;

	start = (uintptr_t)stackAllocator->buffer;
	end = start + (uintptr_t)stackAllocator->bufferLength;
	curr_addr = (uintptr_t)ptr;

	if (!(start <= curr_addr && curr_addr < end))
	{
		assert(0 && "Out of bounds memory address passed to stack allocator (resize)");
		return nullptr;
	}

	if (curr_addr >= start + (uintptr_t)stackAllocator->offset)
	{
		// Treat as a double free
		return nullptr;
	}

	StackAllocatorHeader* header = nullptr;
	header = (StackAllocatorHeader*)(curr_addr - sizeof(StackAllocatorHeader));

	size_t oldSize = header->allocationSize;

	if (oldSize == newSize)
	{
		return ptr;
	}

	new_ptr = StackAlloc(stackAllocator, newSize, alignment);
	// Copies "numBytes" bytes from address "from" to address "to"
	memmove(new_ptr, ptr, oldSize);
	
	return new_ptr;
}


void StackFreeAll(StackAllocator* stackAllocator)
{
	stackAllocator->offset = 0;
}