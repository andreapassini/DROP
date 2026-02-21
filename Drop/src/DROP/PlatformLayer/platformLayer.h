#pragma once

#include "Types/Types.h"

namespace Drop {

int Main(int argc, char** argv);

void* AllocateMemory(
    size_t sizeInBytes
    , void* startingAddress = nullptr
);

}