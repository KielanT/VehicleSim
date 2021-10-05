#pragma once

#include "Misc/ProjectDefines.h"
#include <tinyxml2.h>

namespace Project
{
	class ParseWindowSettings
	{
	public:
		ParseWindowSettings(){}

		WindowProperties LoadWindowSettings(std::string fileName = "data/WinSettings.xml");

	private:
		ERendererType GetRendertype(std::string value);

	private:
		WindowProperties m_Props;
	};
}

