#include "ppch.h"
#include "IRenderer.h"
#include "Graphics/DirectX11/DirectX11Renderer.h"

namespace Project
{
	IRenderer* NewRenderer(const ERendererType type)
	{
		if (type == ERendererType::DirectX11) // Returns the DirectX 11 renderer
		{
			return new DirectX11Renderer();
		}
		else
		{
			return nullptr;
		}
	}
}