#include "Memory/poolAllocator.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "Memory/memoryAllocatorUtils.h"

void PoolInit(
	PoolAllocator* p
	, void* backingBuffer
	, size_t backingBufferLength
	, size_t chunkSize
	, size_t chunkAlignment
) {
	// Align backing buffer to the specified chunk alignment
	uintptr_t initial_start = (uintptr_t)backingBuffer;
	uintptr_t start = AlignForward(initial_start, (uintptr_t)chunkAlignment);
	backingBufferLength -= (size_t)(start - initial_start);

	// Align chunk size up to the required chunkAlignment
	chunkSize = AlignForward(chunkSize, chunkAlignment);

	// Assert that the parameters passed are valid
	assert(chunkSize >= sizeof(PoolAllocatorFreeNode) &&
		"Chunk size is too small");
	assert(backingBufferLength >= chunkSize &&
		"Backing buffer length is smaller than the chunk size");

	// Store the adjusted parameters
	p->buffer = (unsigned char*)backingBuffer;
	p->bufferLength = backingBufferLength;
	p->chunkSize = chunkSize;
	p->head = nullptr; // Free List Head

	// Set up the free list for free chunks
	PoolFreeAll(p);
}

void* PoolAlloc(
	PoolAllocator* p
) {
	// Get latest free node
	PoolAllocatorFreeNode* node = p->head;

	if (node == nullptr) {
		assert(0 && "PoolAllocator allocator has no free memory");
		return nullptr;
	}

	// Pop free node
	p->head = p->head->next;

	// Zero memory by default
	return memset(node, 0, p->chunkSize);
}

void PoolFree(
	PoolAllocator* p
	, void* ptr
) {
	PoolAllocatorFreeNode* node;

	void* start = p->buffer;
	void* end = &p->buffer[p->bufferLength];

	if (ptr == NULL) {
		// Ignore NULL pointers
		return;
	}

	if (!(start <= ptr && ptr < end)) {
		assert(0 && "Memory is out of bounds of the buffer in this pool");
		return;
	}

	// Push free node
	node = (PoolAllocatorFreeNode*)ptr;
	node->next = p->head;
	p->head = node;
}

void PoolFreeAll(
	PoolAllocator* p
) {
	size_t chunk_count = p->bufferLength / p->chunkSize;
	size_t i;

	// what about the first(last) element in the linked list?
	// the next should be nullptr for it
	p->head = nullptr;

	// Set all chunks to be free
	// Reverting the order so p->head will be the next chunk
	for (i = 0; i < chunk_count; i++) {
		void* ptr = &p->buffer[(chunk_count - i - 1) * p->chunkSize];
		PoolAllocatorFreeNode* node = (PoolAllocatorFreeNode*)ptr;
		// Push free node onto thte free list
		node->next = p->head;
		p->head = node;
	}
}