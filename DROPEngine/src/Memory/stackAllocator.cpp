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
	stackAllocator->bufferLenght = backBufferLength;
	stackAllocator->offset = 0LL;
}

void* StackAlloc(
	StackAllocator* stackAllocator
	, size_t size
	, size_t alignment
) {
	

	return nullptr;
}

