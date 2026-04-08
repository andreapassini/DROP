#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

// from https://www.gingerbill.org/code/memory-allocation-strategies/part004.c
// Modified by me

struct PoolAllocatorFreeNode {
	PoolAllocatorFreeNode* next = nullptr;
};

struct PoolAllocator {
	unsigned char* buffer;
	size_t bufferLength;
	size_t chunkSize;

	PoolAllocatorFreeNode* head; // Free List Head
};

void PoolInit(
	PoolAllocator* p
	, void* backingBuffer
	, size_t backingBufferLength
	, size_t chunkSize
	, size_t chunkAlignment
);

void* PoolAlloc(
	PoolAllocator* p
);

void PoolFree(
	PoolAllocator* p
	, void* ptr
);

void PoolFreeAll(
	PoolAllocator* p
);
