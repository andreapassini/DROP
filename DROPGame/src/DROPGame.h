#pragma once

#define DROP_API

#ifdef  DROP_API 
/*Enabled as "export" while compiling the dll project*/
#define DLLEXPORT __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DLLEXPORT __declspec(dllimport)  
#endif

#pragma region GameSideFunctionsDef
// =================== GAME SIDE ===================
#define GAME_DLL_NAME "DropGame.dll"
#define GAME_DLL_TEMP_NAME "DropGame_temp.dll"
#define T_GAME_DLL_NAME TEXT("DropGame.dll")
#define T_GAME_DLL_TEMP_NAME TEXT("DropGame_temp.dll")

#define DLLFUN __cdecl

typedef void(DLLFUN* PRINT_NUMBER)(int);
typedef void(DLLFUN* START_GAME)(void);
typedef void(DLLFUN* UPDATE_GAME)(const float, struct DropEngineCalls*);

struct GameProcAdresses
{
	PRINT_NUMBER PrintNumber = {};
	char* PrintNumberName = "PrintNumber";

	START_GAME StartGame = {};
	char* StartGameName = "StartGame";

	UPDATE_GAME UpdateGame = {};
	char* UpdateGameName = "UpdateGame";
};
#pragma endregion

#pragma region EngineSideFunctionsDef
// =================== ENGINE SIDE ===================
// Engine side Functions Declarations
#define ENGINE_CALL(name) void(*name)(void)
typedef ENGINE_CALL(EngineCall);


#ifdef __cplusplus
extern "C" {
#endif


// API between engine and game
// Game will call this engine functions
struct DLLEXPORT DropEngineCalls
{
	EngineCall engineCall = {};
};

struct DLLEXPORT DropGameInfo {
	float SomeData = 0.0f;
	
};

void DLLEXPORT PrintNumber(const int InNumber);
// Stub
void PrintNumberStub(const int InNumber) {};


void DLLEXPORT StartGame();
// Stub
void StartGameStub() {};


void DLLEXPORT UpdateGame(
	const float deltaTime
	, DropEngineCalls* EngineCalls
);
// Stub
void UpdateGameStub(
	const float deltaTime
	, DropEngineCalls* EngineCalls
) {};



#ifdef __cplusplus
}
#endif
#pragma endregion