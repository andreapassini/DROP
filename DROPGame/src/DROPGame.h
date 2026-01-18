#pragma once

#define DROP_API

#ifdef  DROP_API 
/*Enabled as "export" while compiling the dll project*/
#define DLLEXPORT __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DLLEXPORT __declspec(dllimport)  
#endif

// Engine side Functions Declarations
#define ENGINE_CALL(name) void(*name)(void)
typedef ENGINE_CALL(EngineCall);


#ifdef __cplusplus
extern "C" {
#endif

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