#include "Memory/arenaAllocator.h"

// From Ginger Bill
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
// and modified

#include <cassert>
#include <cstring>
#include "Types/Types.h"
#include "Utils/Log.h"
#include "Utils/assert.h"

#define DEBUG_ARENA_ALLOCATOR 1

void ArenaInit(
	ArenaAllocator* arena
	, void* backingBuffer
	, size_t backingBufferLength
) {
	arena->buffer = (unsigned char*)backingBuffer;
	arena->bufferLenght = backingBufferLength;
	arena->currentOffset = 0;
	arena->previousOffset = 0;
}

void* ArenaAlloc(
	ArenaAllocator* arena
	, size_t size
	, size_t align /*= DEFAULT_ALIGNMENT*/
) {
	// Align 'currentOffset' forward to the specified alignment
	uintptr_t curr_ptr = (uintptr_t)arena->buffer + (uintptr_t)arena->currentOffset;
	uintptr_t offset = AlignForward(curr_ptr, align);
	offset -= (uintptr_t)arena->buffer; // Change to relative offset

	// Check to see if the backing memory has space left
	if (offset + size <= arena->bufferLenght)
	{
		void* ptr = &arena->buffer[offset];

		arena->previousOffset = offset;
		arena->currentOffset = offset + size;

		// Zero new memory by default
		memset(ptr, 0, size);

		return ptr;
	}

	// Return NULL if the arena is out of memory (or handle differently)
	DEBUG_BREAK; // Maybe add another buffer at this point
	return nullptr;
}

void ArenaFree(
	ArenaAllocator* arena
	, void* ptr
) {
	LOG_CORE_INFO("{0} of ptr {1}", __FUNCTION__, (uintptr_t)ptr);

	// Do nothing
}

// useful for resize, avoid reallocating if the resize is on the last chuck
void* ArenaResize(
	ArenaAllocator* arena
	, void* old_memory
	, size_t old_size
	, size_t new_size
	, size_t align /*= DEFAULT_ALIGNMENT*/
) {
	LOG_CORE_INFO("{0} of size {1}", __FUNCTION__, new_size);

	unsigned char* old_mem = (unsigned char*)old_memory;

	assert(IsPowerOfTwo(align));

	if (old_mem == NULL || old_size == 0)
	{
		return ArenaAlloc(arena, new_size, align);
	}
	else if (
		arena->buffer <= old_mem
		&& old_mem < arena->buffer + arena->bufferLenght)
	{
		if (arena->buffer + arena->previousOffset == old_mem)
		{
			arena->currentOffset = arena->previousOffset + new_size;
			if (new_size > old_size)
			{
				// Zero the new memory by default
				memset(
					&arena->buffer[arena->currentOffset]
					, 0
					, new_size - old_size
				);
			}
			return old_memory;
		}
		else
		{
			void* new_memory = ArenaAlloc(arena, new_size, align);
			size_t copy_size = old_size < new_size ? old_size : new_size;
			// Copy across old memory to the new memory
			memmove(new_memory, old_memory, copy_size);
			return new_memory;
		}
	}
	else
	{
		assert(0 && "Memory is out of bounds of the buffer in this arena");
		return NULL;
	}

}

// We could avoid using this by scoping better the Arena Allocator
void ArenaFreeAll(
	ArenaAllocator* arena
) {
	arena->currentOffset = 0;
	arena->previousOffset = 0;
}
