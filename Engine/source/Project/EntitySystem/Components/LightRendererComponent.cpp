#include "ppch.h"
#include "LightRendererComponent.h"
#include "TransformComponent.h"

namespace Project
{
	LightRendererComponent::~LightRendererComponent()
	{
		delete m_Renderer;

		delete m_Entity;

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

	bool LightRendererComponent::Update(float frameTime)
	{
		return true;
	}

	void LightRendererComponent::Render()
	{
		if (m_Renderer->GetRenderType() == ERendererType::DirectX11)
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

			dx11Renderer->PerModelConstants.objectColour = m_Colour;

			dx11Renderer->PerFrameConstants.light1Colour = m_Colour * m_Strength;
			dx11Renderer->PerFrameConstants.light1Position = m_Model->Position();

			m_Model->Render();
		}
	}
}