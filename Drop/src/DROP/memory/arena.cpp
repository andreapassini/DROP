#include "arena.h"

struct Arena
{
	size_t capacity = 0;
	size_t size = 0;
	uint8_t* data = nullptr;
	Arena* nextArena = nullptr;
};

void InitArena(Arena* const arena, const size_t capacity)
{
	arena->data = (uint8_t*)malloc(sizeof(arena->data) * capacity);
	arena->capacity = capacity;
	arena->size = 0;
	arena->nextArena = nullptr;
}

template<typename T>
T* Allocate(Arena* const arena, const size_t size)
{
	T* outPtr = nullptr;

	Arena* currentArena = arena;

	// Arena doesn't have extra size
	assert(currentArena);
	while (!(currentArena->size + size <= currentArena->capacity))
	{
		if (!currentArena->nextArena)
		{
			size_t NextCapacity = currentArena->capacity >= size ? currentArena->capacity : size * RESIZE_MULTIPLIER;
			currentArena->nextArena = (Arena*)malloc(sizeof(Arena));
			InitArena(currentArena->nextArena, NextCapacity);
			std::cout << "Init NextArena w/ cap = " << NextCapacity << std::endl;
		}

		currentArena = currentArena->nextArena;
	}

	outPtr = (T*)(currentArena->data + ((sizeof(currentArena->data) * currentArena->size)));

	currentArena->size += size;

	return outPtr;
}

// Reset the size of the arena
void Reset(Arena* const arena)
{
	if (arena->nextArena)
	{
		Reset(arena->nextArena);
	}
	arena->size = 0;
}

void Free(Arena* arena)
{
	if (arena && arena->data)
	{
		free(arena->data);
		arena->data = nullptr;
		arena->size = 0;
		arena->capacity = 0;

		Arena* currentArena = nullptr;
		currentArena = arena->nextArena;
		while (currentArena && currentArena->data)
		{
			free(currentArena->data);
			currentArena->data = nullptr;
			currentArena->size = 0;
			currentArena->capacity = 0;

			Arena* nextArena = nullptr;
			nextArena = currentArena->nextArena;
			free(currentArena);
			currentArena = nextArena;
		}

		// Maybe the last should be handled by the user
		// it could be stack allocated
		//free(arena);
		//arena = nullptr;
	}
}

void PrintArena(const Arena* const arena)
{
	uint8_t* NextData = (arena->data + ((sizeof(arena->data) * arena->size)));

	std::cout << "Arena.size = " << arena->size <<
		"\n Arena.capacity = " << arena->capacity <<
		"\n Arena.data = " << (void*)arena->data <<
		"\n Arena.NextData = " << (void*)NextData << std::endl;

	if (arena->nextArena)
	{
		PrintArena(arena->nextArena);
	}
}
