#include "ppch.h"

#include "Application.h"
#include "Data/ParseWindowSettings.h"


namespace Project
{
	Application::Application()
	{
		WindowProperties props; // Sets the defualt window properties

		ParseWindowSettings windowSettings;

		props = windowSettings.LoadWindowSettings();

		m_Renderer = NewRenderer(props.RendererType); // Creates the renderer
		if (m_Renderer == nullptr)
		{
			ErrorLogger log;
			log.ErrorMessage(props, "Error Creating the renderer");
		}
		else
		{
			m_Window = std::unique_ptr<IWindow>(IWindow::Create(props)); // Creates the window
			if (m_Window == nullptr)
			{
				ErrorLogger log;
				log.ErrorMessage(props, "Error Creating the window");
			}


			if (!m_Renderer->InitRenderer(props)) // Initializes the renderer
			{
				ErrorLogger log;
				log.ErrorMessage(props, "Error Initializing the renderer");
			}

			m_SceneManager = NewSceneManager(m_Renderer); // Creates the scene manager
			if (m_SceneManager == nullptr)
			{
				ErrorLogger log;
				log.ErrorMessage(props, "Error Creating the scene manager");
			}

		}
	}

	Application::~Application()
	{
		if (m_SceneManager != nullptr)
		{
			m_SceneManager->Release();
			delete m_SceneManager; // Deletes the scene manager

		}

	}

	void Application::Run()
	{
		
		m_Window->Update(m_SceneManager); // Updates the window and sends the scene manager to the window
	}
}