#pragma once

#include "Types/Types.h"

namespace Drop {

int Main(int argc, char** argv);

void UpdatedEngineDLLCheck(
	char* InSourceGameCodeDLLFullPath
	, char* InTempGameCodeDLLFullPath
	, struct EngineDLL* gameDLL
	, uint32 LoadCounter
);

void UpdatedGameDLLCheck(
	char* InSourceGameCodeDLLFullPath
	, char* InTempGameCodeDLLFullPath
	, struct GameDLL* gameDLL
	, uint32 LoadCounter
);

}