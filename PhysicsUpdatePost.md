
# Fast Physics Update


# SoA and SIMD

## Data Layout

Before proceed with SIMD implementation 
I have to convert the data from a AoS layout 

```c++
struct PhysicsComponent{
    vec3 position;
    float mass;
    ..
};

std::vector<PhysicsComponent> physicsComponents;
```
to a SoA layout.
```c++
struct PhysicsComponents {
    float* positions_x;
    float* positions_y;
    float* positions_z;
    float* masses;
    ...
}
```

Having data like this allows for better caching and a better SIMD implementation.


## SIMD

SIMD refers to Single Instruction Multiple Data.
Modern CPUs support vector instructions that allows to operate using larger registers made of 128, 256 or even 512 bits.
These operations are also called 4, 8, 16 wide, since we usually operates on float32 numbers.

### Compile with SIMD instruction set

There are multiple extensions that supports SIMD instructions set like
- SEE (128 bit)
- SEE2 (256 bit)
- AVX (128 bit)
- AVX2 (256 bit)
- AVX512 (512 bit)

Dependig on what you CPU supports.

Building with one of this extensions allows to exploit vector instructions.
Based on my hardware I will use AVX2 for this example.

### Auto Vectorization

One way to use vector instructions is to compile using some auto vectorization pre processor directives. This will tell the compiler to translate those instructions into SIMD instructions. 

```
```

If you take a look at the example abose I just need to add the pre-processor directive and use a simple condition inside the for.
Since this is just a pre processor directive you need to take a look at the disassembly to make sure that the compiler is actually doing what you want.

```
```

### SIMD Intrinsics




# Multi-threading

The next logical optimization is to use multi-threading, by splitting up the work on multiple threads. With this, in theory, I should achieve a speedup equal to the number of cores that my machine supports.

## C++ Futurs

The c++ way to implement this is via Futures. 


Logically the number of threads to use should be the same as the number of threads.




## False Sharing

