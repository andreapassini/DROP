#pragma once

#include <windows.h> 

#define GAME_DLL_NAME "DropGame.dll"
#define GAME_DLL_TEMP_NAME "DropGame_temp.dll"
#define T_GAME_DLL_NAME TEXT("DropGame.dll")
#define T_GAME_DLL_TEMP_NAME TEXT("DropGame_temp.dll")

#define DLLFUN __cdecl

typedef void(DLLFUN* PRINT_NUMBER)(int);
typedef void(DLLFUN* START_GAME)(void);
typedef void(DLLFUN* UPDATE_GAME)(const float);

struct GameDLLProAdresses {
	FILETIME DLLLastWriteTime;

	PRINT_NUMBER PrintNumber = {};
	char* PrintNumberName = "PrintNumber";

	START_GAME StartGame = {};
	char* StartGameName = "StartGame";

	UPDATE_GAME UpdateGame = {};
	char* UpdateGameName = "UpdateGame";

	bool bIsValid = false;
};