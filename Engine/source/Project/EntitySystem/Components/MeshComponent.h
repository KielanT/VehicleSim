#pragma once

#include "Project/Core.h"
#include "Project/EntitySystem/EntityComponent.h"

namespace Project
{
	class P_API MeshComponent : public EntityComponent
	{
	public:
		MeshComponent(std::string& meshFilePath, Entity* entity, TEntityUID UID) : EntityComponent("Mesh", UID, entity)
		{
			m_filePath = meshFilePath;
		}

		virtual bool Update(float frameTime) override;

		const std::string GetMeshPath() { return m_filePath; }

	private:
		std::string m_filePath;
	};
}

