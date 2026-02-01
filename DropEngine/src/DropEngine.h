#pragma once

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

typedef void(DLLFUN* PRINT_NUMBER)(int);
typedef void(DLLFUN* START_ENGINE)(void);
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


static bool g_GameEngineRunning = true;

#ifdef __cplusplus
extern "C" {
#endif

	struct DLLEXPORT DropPlatformCalls
	{
		PlatformCall platformCall = {};
	};

	struct GameEngineCode
	{
		// Data
		bool bGameEngineRunning = true;

		// Func Pointers for API

	};

	void DLLEXPORT StartEngine();
	// Stub
	void StartEngineStub() {};

	void DLLEXPORT UpdateEngine( // Get the DeltaTime from glfwGetTime
		DropPlatformCalls* platformCalls
		, GameProcAdresses* gameCalls
	);
	// Stub
	void UpdateEngineStub(
		DropPlatformCalls* platformCalls
		, GameProcAdresses* gameCalls
	)
	{
	};


#ifdef __cplusplus
}
#endif