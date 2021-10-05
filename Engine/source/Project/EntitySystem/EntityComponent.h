#pragma once

#include "Project/Core.h"

namespace Project
{
	class Entity;

	class P_API EntityComponent
	{
	public:
		EntityComponent(const std::string& type, TEntityUID UID, Entity* entity)
		{
			m_Type = type;
			m_UID = UID;
			m_Entity = entity;
		}

		const std::string& GetType() { return m_Type; }
		const TEntityUID GetUID() { return m_UID; }
		const Entity* GetEntity() { return m_Entity; }

		virtual bool Update(float frameTime) = 0;

	private:
		std::string m_Type;
		TEntityUID m_UID;
		Entity* m_Entity;
	};
}