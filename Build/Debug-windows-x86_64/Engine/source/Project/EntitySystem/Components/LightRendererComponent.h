#pragma once

#include "Project/Core.h"
#include "Project/Lab/Mesh.h"
#include "Project/Lab/Model.h"
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/EntitySystem/Components/MeshComponent.h"
#include "Project/Interfaces/IShader.h"
#include "Project/Interfaces/IState.h"
#include "Graphics/DirectX11/CDirectX11Shader.h"
#include "Graphics/DirectX11/CDirectX11States.h"
#include "Utility/Lab/GraphicsHelpers.h"

namespace Project
{
	class LightRendererComponent : public EntityComponent
	{
	public:
		LightRendererComponent(std::shared_ptr<IRenderer> renderer, Entity* entity, TEntityUID UID, std::shared_ptr<IShader> shader, std::shared_ptr<IState> state, std::string filePath = "media/Flare.jpg",
			CVector3 colour = CVector3(0.0f, 0.0f, 1.0f), float strength = 10.0f) : EntityComponent("Light Renderer", UID, entity)
		{
			m_Renderer = renderer;
			m_Colour = colour;
			m_Strength = strength;

			m_Entity = entity;

			if (entity->GetComponent("Mesh") && m_Renderer->GetRenderType() == ERendererType::DirectX11)
			{
				std::shared_ptr<DirectX11Renderer> dx11Renderer = std::static_pointer_cast<DirectX11Renderer>(m_Renderer);
				MeshComponent* comp = static_cast<MeshComponent*>(entity->GetComponent("Mesh"));

				std::shared_ptr<CDirectX11Shader> dx11Shader = std::static_pointer_cast<CDirectX11Shader>(shader);
				std::shared_ptr<CDirectX11States> dx11State = std::static_pointer_cast<CDirectX11States>(state);



				if (comp->GetMeshPath() != "")
				{

					std::filesystem::path MainPath = std::filesystem::current_path();

					std::filesystem::path meshPath = std::filesystem::current_path().parent_path().append("Engine\\");
					std::filesystem::current_path(meshPath);

					m_Mesh = std::make_shared<Mesh>(dx11Renderer, comp->GetMeshPath());
					m_Model = std::make_shared<Model>(m_Mesh);
					

					if (!LoadTexture(dx11Renderer, filePath, &m_TextureResource, &m_TextureSRV))
					{
						// Error Check here
					}

					std::filesystem::current_path(MainPath);

					m_PixelShader = dx11Shader->GetPixelShader(EPixelShader::LightModelPixelShader);
					m_VertexShader = dx11Shader->GetVertexShader(EVertexShader::BasicTransformVertexShader);

					m_BlendState = dx11State->GetBlendState(EBlendState::AdditiveBlendingState);
					m_DepthStencilState = dx11State->GetDepthStencilState(EDepthStencilState::DepthReadOnlyState);
					m_RasterizerState = dx11State->GetRasterizerState(ERasterizerState::CullNoneState);
					m_SamplerState = dx11State->GetSamplerState(ESamplerState::Anisotropic4xSampler);
				}
				else
				{
					m_Mesh = nullptr;
					m_Model = nullptr;
				}
			}
		}

		~LightRendererComponent();

		virtual bool Update(float frameTime) override;

		void Render();
		std::shared_ptr<Model> GetModel() { return m_Model; }

		const float GetStrength() { return m_Strength; }

	private:
		std::shared_ptr<IRenderer> m_Renderer;

		CVector3 m_Colour;
		float m_Strength;

		Entity* m_Entity;

		std::shared_ptr<Mesh> m_Mesh;
		std::shared_ptr<Model> m_Model;
		ID3D11Resource* m_TextureResource;
		ID3D11ShaderResourceView* m_TextureSRV;

		ID3D11PixelShader* m_PixelShader;
		ID3D11VertexShader* m_VertexShader;

		ID3D11BlendState* m_BlendState;
		ID3D11DepthStencilState* m_DepthStencilState;
		ID3D11RasterizerState* m_RasterizerState;
		ID3D11SamplerState* m_SamplerState;
	};
}
