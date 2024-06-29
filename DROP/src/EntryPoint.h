#pragma once

//
// Adapted from TheCherno Walnut
//

#define WL_PLATFORM_WINDOWS
#ifdef WL_PLATFORM_WINDOWS

extern DROP::GameEngine* DROP::CreateGameEnigne(int argc, char** argv);
bool g_GameEngineRunning = true;

namespace DROP
{

	int Main(int argc, char** argv)
	{
		while (g_GameEngineRunning)
		{
			DROP::GameEngine* ge = DROP::CreateGameEnigne(argc, argv);
			ge->Run();
			delete ge;
		}

		return 0;
	}

}

#ifdef WL_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Walnut::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return DROP::Main(argc, argv);
}

#endif // WL_DIST

#endif // WL_PLATFORM_WINDOWS
