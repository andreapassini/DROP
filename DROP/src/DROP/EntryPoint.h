#pragma once

#ifdef DROP_PLATFORM_WINDOWS

extern Drop::GameEngine* Drop::CreateGameEngine(int argc, char** argv);
bool g_GameEngineRunning = true;

namespace Drop 
{

	int Main(int argc, char** argv)
	{
		while (g_GameEngineRunning)
		{
			Drop::GameEngine* gameEngine = Drop::CreateGameEngine(argc, argv);
			gameEngine->Run();
			delete gameEngine;
		}

		return 0;
	}

}

#ifdef DROP_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Drop::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Drop::Main(argc, argv);
}

#endif // DROP_DIST

#endif // DROP_PLATFORM_WINDOWS
