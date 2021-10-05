#pragma once
#include "Project/Core.h"
#include "IRenderer.h"


namespace Project
{
	// Interface that returns the shaders depending on the renderer in use (currently only using DirectX 11)
	class P_API IShader
	{
	public:
		virtual ~IShader(){}

		// Initialize the shaders 
		virtual bool InitShaders(IRenderer* renderer) = 0;

		// Release the shaders
		virtual void ReleaseShaders() = 0;
	};

	// Sets the shader depending on the renderer
	IShader* SetShader(ERendererType renderertype);
}