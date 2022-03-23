#pragma once

#include "Project/Core.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/SceneSystem/IScene.h"
#include "Project/SceneSystem/ISceneManager.h"


#include "ISceneManager.h"

namespace Project
{
	class CDirectX11SceneManager : public ISceneManager
	{
	public:
		CDirectX11SceneManager(IRenderer* renderer, WindowProperties& props);
		~CDirectX11SceneManager();

		// Loads the first scene in the array (typically index 0)
		virtual bool LoadFirstScene() override;

		// Load scene at index
		virtual void LoadScene(int index);

		// Remove current scene
		virtual void RemoveCurrentScene() override;
		// Remove current scene at index
		virtual void RemoveSceneAtIndex(int index) override;
		
		// Runs the scene loop (Renderer and update)
		virtual void SceneLoop(float frameTime) override;
		
		// Gets the the current scene index
		virtual const int GetCurrentSceneIndex() override { return m_SceneIndex; }

		virtual IRenderer* GetRenderer() override { return m_Renderer; }

		WindowProperties GetWindowsProperties() { return m_Props; }

	private:
		// Creates the scenes
		virtual void CreateScenesFromFile() override;
		virtual void CreateSceneFromObject() override; // TEMP
		
		// Renders the scene
		virtual void RenderScene() override;
		virtual void RenderSceneFromCamera() override;

	private:
		IRenderer* m_Renderer; // Memeber variable for the renderer in use
		WindowProperties m_Props; // Member variable for the window properties
		std::vector<IScene*> m_Scenes; // Member variable for the scene array

		int m_SceneIndex; // Memeber variable for the tracking the current scene index
	};
}


