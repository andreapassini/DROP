
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


### SIMD Times

#### 10'000
SIMD_SoA_physicsStepAverage: 0.018901
SIMD_ByHand_SoA_physicsStepDuration: 0.018287

#### 100'000
SIMD_SoA_physicsStepAverage: 0.218797
SIMD_ByHand_SoA_physicsStepDuration: 0.184417

#### 1'000'000
SIMD_SoA_physicsStepAverage: 2.324659
SIMD_ByHand_SoA_physicsStepDuration: 1.947480


# Multi-threading

The next logical optimization is to use multi-threading, by splitting up the work on multiple threads. With this, in theory, I should achieve a speedup equal to the number of cores that my machine supports.

## Times

### 1'000'000

#### 8 Threads
MultiThread_SoA_physicsStepAverage: 3.388983
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 1.091182

#### 16 Threads
MultiThread_SoA_physicsStepAverage: 3.436673
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 1.099777

#### 24 Threads
MultiThread_SoA_physicsStepAverage: 3.436673
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 1.173103

#### 32 Threads
MultiThread_SoA_physicsStepAverage: 3.334618
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 1.183510

#### 40 Threads
MultiThread_SoA_physicsStepAverage: 3.188586
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 1.092315

#### 48 Threads
MultiThread_SoA_physicsStepAverage: 3.039289
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 1.089883

#### 56 Threads
MultiThread_SoA_physicsStepAverage: 2.960849
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 1.173925

#### 64 Threads
MultiThread_SoA_physicsStepAverage: 2.793264
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 1.105237


### 1'000'000 LAPTOP

#### 8 Threads LAPTOP
MultiThread_SoA_physicsStepAverage: 3.310043
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 3.090865

#### 16 Threads LAPTOP
MultiThread_SoA_physicsStepAverage: 3.272521
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 2.947581

#### 24 Threads LAPTOP
MultiThread_SoA_physicsStepAverage: 2.545941
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 2.673936

#### 32 Threads LAPTOP
MultiThread_SoA_physicsStepAverage: 2.829313
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 2.671933

#### 40 Threads LAPTOP
MultiThread_SoA_physicsStepAverage: 2.525473
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 2.618814

#### 48 Threads LAPTOP
MultiThread_SoA_physicsStepAverage: 2.512836
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 2.807498

#### 56 Threads LAPTOP
MultiThread_SoA_physicsStepAverage: 2.796912
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 2.774405

#### 64 Threads LAPTOP
MultiThread_SoA_physicsStepAverage: 2.686143
MultiThread_SoA_SIMD_ByHand_physicsStepDurationAverage: 2.712607

## C++ Futurs

The c++ way to implement this is via Futures. 


Logically the number of threads to use should be the same as the number of threads.




## False Sharing

