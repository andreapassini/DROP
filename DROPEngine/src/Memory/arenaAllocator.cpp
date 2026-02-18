#include "Memory/arenaAllocator.h"

// From Ginger Bill
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
// and modified

#include <cassert>
#include <vcruntime_string.h>
#include "Types/Types.h"


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
		memset(ptr, 0, size); // this will override the fucking arena values GODDAMIT if not assigned in the right way
		return ptr;
	}
	// Return NULL if the arena is out of memory (or handle differently)
	return nullptr;
}

void ArenaFree(
	ArenaAllocator* arena
	, void* ptr
) {
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



// UTILS ----------------------

// https://stackoverflow.com/questions/40941825/understanding-void-against-intptr-t-and-uintptr-t
// If you need to do numeric computations on pointers, use those types. 
// Otherwise, if you just need to store "a pointer to something," use a void*.
bool IsPowerOfTwo(
	uintptr_t x
)
{
	return (x & (x - 1)) == 0;
}

// To align a memory address to the specified alignment is simple modulo arithmetic. 
// You are looking to find how many bytes forward you need to go 
// in order for the memory address is a multiple of the specified alignment.
uintptr_t AlignForward(
	uintptr_t ptr
	, size_t align
) {
	uintptr_t p, a, modulo;

	assert(IsPowerOfTwo(align));

	p = ptr;
	a = (uintptr_t)align;
	// Same as (p % a) but faster as 'a' is a power of two
	modulo = p & (a - 1);

	if (modulo != 0)
	{
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}