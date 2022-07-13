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
		// Constructor sets all the required data for rendering
		RendererComponent(bool isRendered, std::shared_ptr<IRenderer> renderer, Entity* entity, TEntityUID UID, std::shared_ptr<IShader> shader, std::shared_ptr<IState> state,
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
				std::shared_ptr<DirectX11Renderer> dx11Renderer = std::static_pointer_cast<DirectX11Renderer>(m_Renderer);
				MeshComponent* comp = static_cast<MeshComponent*>(entity->GetComponent("Mesh"));
			

				std::shared_ptr<CDirectX11Shader> dx11Shader = std::static_pointer_cast<CDirectX11Shader>(shader);
				std::shared_ptr<CDirectX11States> dx11State = std::static_pointer_cast<CDirectX11States>(state);

				if (comp->GetMeshPath() != "")
				{
					// Changes the filepath to the correct path
					std::filesystem::path MainPath = std::filesystem::current_path();

					std::filesystem::path meshPath = std::filesystem::current_path().parent_path().append("Engine\\");

					std::filesystem::current_path(meshPath); // Sets the current path to the mesh path
					m_Mesh = std::make_shared<Mesh>(dx11Renderer, comp->GetMeshPath());


					m_Model = std::make_shared<Model>(m_Mesh);


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

		// Will render the models
		void Render();
		std::shared_ptr<Model> GetModel() { return m_Model; }

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

		int GetNumberTriangles() { return m_Mesh->GetNumberTriangles(); }
		std::vector<unsigned int> GetIndices() { return m_Mesh->GetIndices(); }
		
		bool GetIsRendered() { return m_isRendered; }

	private:
		std::shared_ptr<IRenderer> m_Renderer;

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

		bool m_isRendered;

		
	};
}
