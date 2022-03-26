#include "ppch.h"
#include "RendererComponent.h"
#include "Utility/Lab/GraphicsHelpers.h"

namespace Project
{
	RendererComponent::~RendererComponent()
	{
		delete m_Renderer;

		delete m_Mesh;
		delete m_Model;

		if (m_TextureResource) m_TextureResource->Release();
		if (m_TextureSRV) m_TextureSRV->Release();

		if (m_PixelShader) m_PixelShader->Release();
		if (m_VertexShader) m_VertexShader->Release();

		if (m_BlendState) m_BlendState->Release();
		if (m_DepthStencilState) m_DepthStencilState->Release();
		if (m_RasterizerState) m_RasterizerState->Release();
		if (m_SamplerState) m_SamplerState->Release();
	}

	bool RendererComponent::Update(float frameTime)
	{
		return true;
	}

	void RendererComponent::Render()
	{

		if( m_Model != nullptr && m_Renderer->GetRenderType() == ERendererType::DirectX11)
		{
			DirectX11Renderer* dx11Renderer = static_cast<DirectX11Renderer*>(m_Renderer);

			dx11Renderer->GetDeviceContext()->VSSetShader(m_VertexShader, nullptr, 0);
			dx11Renderer->GetDeviceContext()->PSSetShader(m_PixelShader, nullptr, 0);

			// Select the approriate textures and sampler to use in the pixel shader
			dx11Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_TextureSRV); // First parameter must match texture slot number in the shader
			dx11Renderer->GetDeviceContext()->PSSetSamplers(0, 1, &m_SamplerState);

			// States - no blending, normal depth buffer and culling
			dx11Renderer->GetDeviceContext()->OMSetBlendState(m_BlendState, nullptr, 0xffffff);
			dx11Renderer->GetDeviceContext()->OMSetDepthStencilState(m_DepthStencilState, 0);
			dx11Renderer->GetDeviceContext()->RSSetState(m_RasterizerState);

			m_Model->Render();

		}
	}

	void RendererComponent::SetTexture(std::string texturePath)
	{
		if (m_Renderer->GetRenderType() == ERendererType::DirectX11)
		{
			DirectX11Renderer* dx11Renderer = static_cast<DirectX11Renderer*>(m_Renderer);
			if (!LoadTexture(dx11Renderer, texturePath, &m_TextureResource, &m_TextureSRV))
			{
				int i = 0;
				// Error Check here
			}
		}
	}

	void RendererComponent::SetShaders(ID3D11PixelShader* ps, ID3D11VertexShader* vs)
	{
		SetPixelShader(ps);
		SetVertexShader(vs);
	}
}