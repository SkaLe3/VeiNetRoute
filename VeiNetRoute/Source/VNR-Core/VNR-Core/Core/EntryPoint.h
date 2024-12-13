#pragma once
#include "Application.h"
#include "VNR-Core/IO/Log.h"

extern VNR::Application* VNR::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace VNR
{
	int Main(int argc, char** argv)
	{
		Log::Init();
		while (g_ApplicationRunning)
		{
			VNR::Application* app = VNR::CreateApplication(argc, argv);
			app->Run();
			delete app;
		}
		return 0;
	}
}

#ifdef VNR_SHIPPING
#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return VNR::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return VNR::Main(argc, argv);
}

#endif // VNR_DIST