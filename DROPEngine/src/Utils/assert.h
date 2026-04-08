#pragma once

#if  defined(DROP_DEBUG)

#ifdef DROP_PLATFORM_WINDOWS 
#include "Windows.h"
//#include "debugapi.h" // this cause compile errors
// __debugBreak is not working 
// https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-debugbreak
#define DEBUG_BREAK DebugBreak();
#define ensure(a) \
	if(!a) \
	{ \
		DEBUG_BREAK \
	}

#endif // DROP_PLATFORM_WINDOWS

#else // if(!defined DROP_DEBUG)

#define DEBUG_BREAK

#define ensure(a)

#endif //DEBUG