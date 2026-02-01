#pragma once

#include <windows.h> 

#include "DropEngine.h"
#include "DropGame.h"

struct EngineDLL
{
	FILETIME DLLastWriteTime;

	EngineProcAdresses procAdresses;

	bool bIsValid = false;
};

struct GameDLL
{
	FILETIME DLLastWriteTime;

	GameProcAdresses procAdresses;

	bool bIsValid = false;
};
