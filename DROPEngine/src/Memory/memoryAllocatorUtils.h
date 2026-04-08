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
) {
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

	if (modulo != 0) {
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}

uintptr_t AlignForward(
	uintptr_t ptr
	, uintptr_t align
) {
	uintptr_t p, a, modulo;

	assert(IsPowerOfTwo(align));

	p = ptr;
	a = align;
	// Same as (p % a) but faster as 'a' is a power of two
	modulo = p & (a - 1);

	if (modulo != 0) {
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}

uintptr_t AlignForward(
	size_t ptr
	, size_t align
) {
	size_t p, a, modulo;

	assert(IsPowerOfTwo((uintptr_t)align));

	p = ptr;
	a = align;
	// Same as (p % a) but faster as 'a' is a power of two
	modulo = p & (a - 1);

	if (modulo != 0) {
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}