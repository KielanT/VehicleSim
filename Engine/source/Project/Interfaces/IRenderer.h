#pragma once

#include "Project/Core.h"


namespace Project
{
	// Renderer interface for creating Renderers using different graphic APIs (currently only using DirectX11)
	class P_API IRenderer
	{
	public:
		virtual ~IRenderer(){}

		// Initialize the renderer
		virtual bool InitRenderer(WindowProperties& props) = 0; 
		 
		// Shutdown the renderer
		virtual void ShutdownRenderer() = 0;

		// Get the renderer type
		virtual const ERendererType GetRenderType() = 0;

		// Get the Window Properties
		virtual WindowProperties GetWindowsProperties() = 0;
	};

	// Returns the renderer base on the type selected
	IRenderer* NewRenderer(const ERendererType type);
}