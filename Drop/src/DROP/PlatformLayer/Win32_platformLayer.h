#pragma once

#include <windows.h> 

#include "DROPGame.h"

struct GameDLLProcAdresses
{
	FILETIME DLLastWriteTime;

	GameProcAdresses gameProcAdresses;

	bool bIsValid = false;
};