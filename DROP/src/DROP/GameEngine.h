#pragma once

#define GAME_DLL_NAME TEXT("DropGame.dll")

#define DLLFUN __cdecl

typedef void(DLLFUN* PRINT_NUMBER)(int);
typedef void(DLLFUN* START_GAME)(void);
typedef void(DLLFUN* UPDATE_GAME)(const float);

struct GameDLLProAdresses {
	bool bIsValid = false;

	PRINT_NUMBER PrintNumber = {};
	char* PrintNumberName = "PrintNumber";

	START_GAME StartGame = {};
	char* StartGameName = "StartGame";

	UPDATE_GAME UpdateGame = {};
	char* UpdateGameName = "UpdateGame";
};