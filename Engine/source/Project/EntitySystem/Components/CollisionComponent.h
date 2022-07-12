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
		CollisionComponent(IRenderer* renderer, const std::string& meshPath, Entity* entity, TEntityUID UID) : EntityComponent("CollisionMesh", UID, entity)
		{
			m_Renderer = renderer;
			if (m_Renderer->GetRenderType() == ERendererType::DirectX11)
			{
				DirectX11Renderer* dx11Renderer = static_cast<DirectX11Renderer*>(m_Renderer);

				std::filesystem::path MainPath = std::filesystem::current_path();

				std::filesystem::path Path = std::filesystem::current_path().parent_path().append("Engine\\");

				std::filesystem::current_path(Path); // Sets the current path to the mesh path

				m_Mesh = new Mesh(dx11Renderer, meshPath);
				m_Model = new Model(m_Mesh);
			}
		}
		~CollisionComponent()
		{
			//delete m_Renderer;
			delete m_Mesh;
			delete m_Model;
		}
		
		virtual bool Update(float frameTime) override;

		unsigned int GetNumberOfVertices(int index) { return m_Mesh->GetNumberVertices(index); }
		std::vector<CVector3> GetVertices(int index) { return m_Mesh->GetVertices(); }

		int GetNumberTriangles(int index) { return m_Mesh->GetNumberTriangles(index); }
		std::vector<unsigned int> GetIndices() { return m_Mesh->GetIndices(); }
	private:
		IRenderer* m_Renderer;
		Mesh* m_Mesh;
		Model* m_Model;
		
	};

}