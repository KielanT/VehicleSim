#pragma once

#include "Project/Core.h"

// TODO: add a way to log errors without needing window handle

namespace Project
{
	// Class with ways to error log
	class P_API ErrorLogger
	{
	public:
		// General error log
		void ErrorMessage(const WindowProperties props, const std::string ErrorMessage); 
	};
}
