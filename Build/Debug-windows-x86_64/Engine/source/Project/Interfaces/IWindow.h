#pragma once

#include "Misc/ProjectDefines.h"
#include "Project/Core.h"
#include <memory>
#include "Project/SceneSystem/ISceneManager.h"

namespace Project
{
	// Window interface used for creating windows for each platform (currently only supports windows)
	class P_API IWindow
	{
	public:
		virtual ~IWindow() {}

		// Updates the window with the scene
		virtual void Update(std::shared_ptr<ISceneManager> m_SceneManager) = 0;

		// Returns the window width
		virtual unsigned int GetWidth() const = 0;

		// Returns the window height
		virtual unsigned int GetHeight() const = 0;

		// Returns the window propities
		virtual WindowProperties GetWindowProperties() const = 0;

		// Creates the window
		static std::unique_ptr<IWindow> Create(WindowProperties& props);
	};
}