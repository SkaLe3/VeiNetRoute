#include "WindowsUtils.h"
#include <GLFW/glfw3.h>

namespace VNR
{
	float Time::GetTime()
	{
		return glfwGetTime();
	}
}

