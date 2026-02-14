#pragma once

#include <cstdint>

// -----------------------------------------
// DROP Ints
//typedef signed char        int8_t;
//typedef short              int16_t;
//typedef int                int32_t;
//typedef long long          int64_t;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
#define INT32_ERROR_VALUE _I32_MAX
typedef int64_t int64;
#define INT64_ERROR_VALUE _I64_MAX
typedef int8 i8;
typedef int16 i16;
typedef int32 i32;
#define I32_ERROR_VALUE INT32_ERROR_VALUE
typedef int64 i64;
#define I64_ERROR_VALUE INT64_ERROR_VALUE

//typedef unsigned char      uint8_t;
//typedef unsigned short     uint16_t;
//typedef unsigned int       uint32_t;
//typedef unsigned long long uint64_t;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uint8 u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;


// -----------------------------------
// DROP Floats
typedef float float32;
typedef double float64;

#define PRINT_FLOAT32_2 %.2f
#define PRINT_FLOAT_2 PRINT_FLOAT32_2

#define PRINT_FLOAT32_3 %.3f
#define PRINT_FLOAT_3 PRINT_FLOAT32_3

#define PRINT_FLOAT32_4 %.4f
#define PRINT_FLOAT_4 PRINT_FLOAT32_4