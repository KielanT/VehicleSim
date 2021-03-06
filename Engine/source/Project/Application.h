#pragma once
#include "Core.h"
#include "Interfaces/IWindow.h"
#include "Interfaces/IRenderer.h"

#include "SceneSystem/ISceneManager.h"

namespace Project
{
	class P_API Application
	{
	public:
		Application();
		~Application();

		void Run(); // Function for running the application


	private:
		std::unique_ptr<IWindow> m_Window; // Variable for the window
		IRenderer* m_Renderer = nullptr; // Variable for the renderer

		ISceneManager* m_SceneManager; // Variable for the scene manager

	};

	Application* CreateApplication();
}
