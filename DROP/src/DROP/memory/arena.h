#pragma once

#include <assert.h>
#include <iostream>

#define RESIZE_MULTIPLIER 1.5f

struct Arena
{
	size_t capacity = 0;
	size_t size = 0;
	uint8_t* data = nullptr;
	Arena* nextArena = nullptr;
};

void InitArena(Arena* const arena, const size_t capacity);

template<typename T>
T* Allocate(Arena* const arena, const size_t size);

// Reset the size of the arena
void Reset(Arena* const arena);

void Free(Arena* arena);

void PrintArena(const Arena* const arena);