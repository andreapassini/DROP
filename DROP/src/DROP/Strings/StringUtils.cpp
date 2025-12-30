#include "Drop/Strings/stringUtils.h"
#include <cassert>

void ConcatStrings(
    size_t SourceACount, char* SourceA
    , size_t SourceBCount, char* SourceB
    , size_t DestCount, char* Dest
) {
    // dest must be SourceACount + SourceBCount + 1 (for the null terminator)
    if (DestCount < SourceACount + SourceBCount + 1)
    {
        assert(0);
        return;
    }

    for (int i = 0; i < SourceACount; ++i)
    {
        *Dest++ = *SourceA++;
    }

    for (int i = 0; i < SourceBCount; ++i)
    {
        *Dest++ = *SourceB++;
    }

    *Dest++ = 0;
}