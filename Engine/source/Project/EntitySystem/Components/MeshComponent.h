#pragma once

#include "Project/Core.h"
#include "Project/EntitySystem/EntityComponent.h"

namespace Project
{
	class P_API MeshComponent : public EntityComponent
	{
	public:
		MeshComponent(std::string& meshFilePath, Entity* entity, TEntityUID UID, int index = 0) : EntityComponent("Mesh", UID, entity)
		{
			m_filePath = meshFilePath;
			m_Index = index;
		}

		virtual bool Update(float frameTime) override;

		const std::string GetMeshPath() { return m_filePath; }

		~MeshComponent(){}

	private:
		std::string m_filePath;
		int m_Index;
	};
}

