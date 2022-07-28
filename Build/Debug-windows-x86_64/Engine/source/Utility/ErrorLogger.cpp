#include "ppch.h"
#include "ErrorLogger.h"

namespace Project
{
	void ErrorLogger::ErrorMessage(const WindowProperties props, const std::string ErrorMessage)
	{
		// Creates a dialoge box with the error message and a OK button
		MessageBoxA(props.Hwnd, ErrorMessage.c_str(), NULL, MB_OK);
	}
}
