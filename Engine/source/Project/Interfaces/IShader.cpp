#include "ppch.h"
#include "IShader.h"
#include "Graphics/DirectX11/CDirectX11Shader.h"

namespace Project
{
	std::unique_ptr<IShader> SetShader(ERendererType renderertype)
	{
		if (renderertype == ERendererType::DirectX11) // Returns the DirectX 11 shaders
		{
			return std::make_unique<CDirectX11Shader>();
		}
		else
		{
			return nullptr;
		}
	}
}