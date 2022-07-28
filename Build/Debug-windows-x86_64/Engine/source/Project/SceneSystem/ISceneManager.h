#pragma once

#include "Project/Interfaces/IRenderer.h"

namespace Project
{
	// Interface class for the scene manager
	class ISceneManager
	{
	public:

		virtual ~ISceneManager(){}
		// Loads the first scene in the array (typically index 0)
		virtual bool LoadFirstScene() = 0; 

		// Load scene at index
		virtual void LoadScene(int index) = 0; 

		// Remove current scene
		virtual void RemoveCurrentScene() = 0;
		// Remove current scene at index
		virtual void RemoveSceneAtIndex(int index) = 0;

		// Runs the scene loop (Renderer and update)
		virtual void SceneLoop(float frameTime) = 0;

		// Gets the the current scene index
		virtual const int GetCurrentSceneIndex() = 0;

		virtual std::shared_ptr<IRenderer> GetRenderer() = 0;

		virtual void Release() = 0;

	private:
		// Creates the scenes
		virtual void CreateScenesFromFile() = 0;
		virtual void CreateSceneFromObject() = 0; // TEMP
		
		// Renders the scene
		virtual void RenderScene() = 0;
		virtual void RenderSceneFromCamera() = 0;
	};

	// Used for creating the scene manager depending on the renderer 
	std::shared_ptr<ISceneManager> NewSceneManager(std::shared_ptr<IRenderer> renderer);
}
