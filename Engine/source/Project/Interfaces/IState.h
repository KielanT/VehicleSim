#pragma once
#include "Project/Core.h"
#include "IRenderer.h"

namespace Project
{ 
	// State interface used for creating states depending on the renderer
	class P_API IState
	{
	public:
		virtual ~IState() {}

		// Initialize the states 
		virtual bool InitStates(std::shared_ptr<IRenderer> renderer) = 0;

		// Release the states
		virtual void ReleaseStates() = 0;
	};

	// Returns the states depending on the renderer
	std::unique_ptr<IState> SetStates(ERendererType type);
}