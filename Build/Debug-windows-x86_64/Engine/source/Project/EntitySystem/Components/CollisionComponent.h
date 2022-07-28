#pragma once

#include "Project/Lab/Mesh.h"
#include "Project/Lab/Model.h"
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"

/************************************************************/
/******		Collision component only used for		 ********/
/******		  the vehicle component	                 ********/
/******		  Needs better achitecture               ********/
/************************************************************/

namespace Project
{
	class CollisionComponent : public EntityComponent
	{
	public:
		CollisionComponent(std::shared_ptr<IRenderer> renderer, const std::string& meshPath, Entity* entity, TEntityUID UID) : EntityComponent("CollisionMesh", UID, entity)
		{
			m_Renderer = renderer;
			if (m_Renderer->GetRenderType() == ERendererType::DirectX11)
			{
				std::shared_ptr<DirectX11Renderer> dx11Renderer = std::static_pointer_cast<DirectX11Renderer>(m_Renderer);

				std::filesystem::path MainPath = std::filesystem::current_path();

				std::filesystem::path Path = std::filesystem::current_path().parent_path().append("Engine\\");

				std::filesystem::current_path(Path); // Sets the current path to the mesh path

				m_Mesh = std::make_shared<Mesh>(dx11Renderer, meshPath);
				m_Model = std::make_shared<Model>(m_Mesh);
			}
		}
		~CollisionComponent()
		{
		}
		
		virtual bool Update(float frameTime) override;

		unsigned int GetNumberOfVertices(int index) { return m_Mesh->GetNumberVertices(index); }
		std::vector<CVector3> GetVertices(int index) { return m_Mesh->GetVertices(); }

		int GetNumberTriangles() { return m_Mesh->GetNumberTriangles(); }
		std::vector<unsigned int> GetIndices() { return m_Mesh->GetIndices(); }
	private:
		std::shared_ptr<IRenderer> m_Renderer;
		std::shared_ptr<Mesh> m_Mesh;
		std::shared_ptr<Model> m_Model;
		
	};

}