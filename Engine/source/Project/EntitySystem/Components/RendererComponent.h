#pragma once

#include "Project/Core.h"
#include "Project/Lab/Mesh.h"
#include "Project/Lab/Model.h"
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/EntitySystem/Components/MeshComponent.h"
//#include "Project/EntitySystem/Components/TransformComponent.h"
#include "Project/Interfaces/IShader.h"
#include "Graphics/DirectX11/CDirectX11Shader.h"
#include "Project/Interfaces/IState.h"
#include "Graphics/DirectX11/CDirectX11States.h"

namespace Project
{
	class P_API RendererComponent : public EntityComponent
	{
	public:
		RendererComponent(bool isRendered,  IRenderer* renderer, Entity* entity, TEntityUID UID, IShader* shader, IState* state,
			std::string filePath = "media/BasicTex.png",
			EPixelShader pixelShader = EPixelShader::PixelLightingPixelShader,
			EVertexShader vertexShader = EVertexShader::PixelLightingVertexShader,
			EBlendState blendState = EBlendState::NoBlendingState,
			EDepthStencilState depthStencilState = EDepthStencilState::UseDepthBufferState,
			ERasterizerState rasterizerState = ERasterizerState::CullNoneState,
			ESamplerState samplerState = ESamplerState::Anisotropic4xSampler) : EntityComponent("Renderer", UID, entity)
		{
			m_isRendered = isRendered;
			m_Renderer = renderer;

			if (entity->GetComponent("Mesh") && m_Renderer->GetRenderType() == ERendererType::DirectX11)
			{
				DirectX11Renderer* dx11Renderer = static_cast<DirectX11Renderer*>(m_Renderer);
				MeshComponent* comp = static_cast<MeshComponent*>(entity->GetComponent("Mesh"));
			

				CDirectX11Shader* dx11Shader = static_cast<CDirectX11Shader*>(shader);
				CDirectX11States* dx11State = static_cast<CDirectX11States*>(state);

				if (comp->GetMeshPath() != "")
				{

					std::filesystem::path MainPath = std::filesystem::current_path();

					std::filesystem::path meshPath = std::filesystem::current_path().parent_path().append("Engine\\");

					std::filesystem::current_path(meshPath); // Sets the current path to the mesh path
					m_Mesh = new Mesh(dx11Renderer, comp->GetMeshPath());


					m_Model = new Model(m_Mesh);


					SetTexture(filePath);
					std::filesystem::current_path(MainPath);

					SetShaders(dx11Shader->GetPixelShader(pixelShader),
						dx11Shader->GetVertexShader(vertexShader));

					SetBlendState(dx11State->GetBlendState(blendState));
					SetDepthStencilState(dx11State->GetDepthStencilState(depthStencilState));
					SetRasterizerState(dx11State->GetRasterizerState(rasterizerState));
					SetSamplerState(dx11State->GetSamplerState(samplerState));
				}
				else
				{
					m_Mesh = nullptr;
					m_Model = nullptr;
				}
			}
			else
			{
				m_Mesh = nullptr;
				m_Model = nullptr;
			}
		}

		~RendererComponent();

		virtual bool Update(float frameTime) override;

		void Render();
		Model* GetModel() { return m_Model; }

		void SetIsRendered(bool isRendered) { m_isRendered = isRendered; }

		void SetTexture(std::string texturePath);

		void SetShaders(ID3D11PixelShader* ps, ID3D11VertexShader* vs);
		void SetPixelShader(ID3D11PixelShader* ps) { m_PixelShader = ps; }
		void SetVertexShader(ID3D11VertexShader* vs) { m_VertexShader = vs; }

		void SetBlendState(ID3D11BlendState* bs) { m_BlendState = bs; }
		void SetDepthStencilState(ID3D11DepthStencilState* dss) { m_DepthStencilState = dss; }
		void SetRasterizerState(ID3D11RasterizerState* rs) { m_RasterizerState = rs; }
		void SetSamplerState(ID3D11SamplerState* ss) { m_SamplerState = ss; }

		unsigned int GetNumberOfVertices(int index) { return m_Mesh->GetNumberVertices(index); }
		std::vector<CVector3> GetVertices() { return m_Mesh->GetVertices(); }
		
		bool GetIsRendered() { return m_isRendered; }

	private:
		IRenderer* m_Renderer;

		Mesh* m_Mesh;
		Model* m_Model;

		ID3D11Resource* m_TextureResource;
		ID3D11ShaderResourceView* m_TextureSRV;

		ID3D11PixelShader* m_PixelShader;
		ID3D11VertexShader* m_VertexShader;

		ID3D11BlendState* m_BlendState;
		ID3D11DepthStencilState* m_DepthStencilState;
		ID3D11RasterizerState* m_RasterizerState;
		ID3D11SamplerState* m_SamplerState;

		bool m_isRendered;

		
	};
}
