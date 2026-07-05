#pragma once

#include <cassert>
#include "Types/Types.h"

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

// https://stackoverflow.com/questions/40941825/understanding-void-against-intptr-t-and-uintptr-t
// If you need to do numeric computations on pointers, use those types. 
// Otherwise, if you just need to store "a pointer to something," use a void*.
bool IsPowerOfTwo(
	uintptr_t x
);

// To align a memory address to the specified alignment is simple modulo arithmetic. 
// You are looking to find how many bytes forward you need to go 
// in order for the memory address is a multiple of the specified alignment.
uintptr_t AlignForward(
	uintptr_t ptr
	, size_t align
);


size_t CalulatePaddingWithHeader(
	uintptr_t ptr
	, size_t header_size
	, uintptr_t alignment = DEFAULT_ALIGNMENT
);