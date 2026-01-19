#pragma once

//#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#define DROP_API

#ifdef  DROP_API 
/*Enabled as "export" while compiling the dll project*/
#define DLLEXPORT __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DLLEXPORT __declspec(dllimport)  
#endif

// Engine side Functions Declarations
#define PLATFORM_CALL(name) void(*name)(void)
typedef PLATFORM_CALL(PlatformCall);

static bool g_GameEngineRunning = true;

#ifdef __cplusplus
extern "C" {
#endif



struct DLLEXPORT PlatformCalls
{
	PlatformCall platformCall = {};
};


void DLLEXPORT StartEngine();
// Stub
void StartEngine() {};


void DLLEXPORT UpdateEngine(
	const float deltaTime
	, PlatformCalls* platformCalls
);
// Stub
void UpdateEngineStub(
	const float deltaTime
	, PlatformCalls* platformCalls
) {};


void DLLEXPORT UpdatedGameDLL(
);
// Stub
void UpdatedGameDLLStub(
) {};



#ifdef __cplusplus
}
#endif