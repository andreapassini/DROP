#pragma once

#include "Types/Types.h"

#define DROP_ENGINE_API

#ifdef  DROP_ENGINE_API 
/*Enabled as "export" while compiling the dll project*/
#define DLLEXPORT __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DLLEXPORT __declspec(dllimport)  
#endif

#pragma region EngineSideFunctionsDef
// =================== GAME SIDE ===================
#define ENGINE_DLL_NAME "DropEngine.dll"
#define ENGINE_DLL_TEMP_NAME "DropEngine_temp.dll"
#define T_ENGINE_DLL_NAME TEXT("DropEngine.dll")
#define T_ENGINE_DLL_TEMP_NAME TEXT("DropEngine_temp.dll")

#define DLLFUN __cdecl

struct GameProcAdresses;
struct DropPlatformCalls;

typedef void(DLLFUN* START_ENGINE)(DropPlatformCalls*);
typedef void(DLLFUN* UPDATE_ENGINE)(DropPlatformCalls*, GameProcAdresses*);

struct EngineProcAdresses
{
	START_ENGINE StartEngine = {};
	char* StartEngineName = "StartEngine";

	UPDATE_ENGINE UpdateEngine = {};
	char* UpdateEngineName = "UpdateEngine";
};
#pragma endregion

// Platform side Functions Declarations
#define PLATFORM_CALL(name) void(*name)(void)
typedef PLATFORM_CALL(PlatformCall);
#define PLATFORM_ALLOC_CALL(name) void*(*name)(size_t, void*)
typedef PLATFORM_ALLOC_CALL(AllocateMemory);

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

static bool g_GameEngineRunning = true;

#ifdef __cplusplus
extern "C" {
#endif

	struct DLLEXPORT DropPlatformCalls
	{
		PlatformCall platformCall = {};
		AllocateMemory allocateMemory = {};
	};

	struct EngineMemory
	{
		void* memory = nullptr;
		int64 sizeInBytes = 0LL;
	};

	struct GameEngineCode
	{
		// Data
		bool bGameEngineRunning = true;

		// Func Pointers for API

	};

	void DLLEXPORT StartEngine(DropPlatformCalls* platformCalls);
	// Stub
	void StartEngineStub(DropPlatformCalls* platformCalls) {};

	void DLLEXPORT UpdateEngine( // Get the DeltaTime from glfwGetTime
		DropPlatformCalls* platformCalls
		, GameProcAdresses* gameCalls
	);
	// Stub
	void UpdateEngineStub(
		DropPlatformCalls* platformCalls
		, GameProcAdresses* gameCalls
	)
	{};


#ifdef __cplusplus
}
#endif