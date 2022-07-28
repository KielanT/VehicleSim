#pragma once

#include "Project/Core.h"
#include "EntityComponent.h"

namespace Project
{


	class P_API Entity
	{
	public:
		Entity(TEntityUID UID, const std::string& name);

		~Entity()
		{
			RemoveAllComponent();
		}
	public:
		TEntityUID GetUID() { return m_UID; }
		std::string& GetName() { return m_Name; }

		void AddComponent(EntityComponent* component);
		EntityComponent* GetComponent(TEntityUID UID);
		EntityComponent* GetComponent(const std::string& type);
		bool RemoveComponent(TEntityUID UID);
		bool RemoveComponent(const std::string& type);

		void RemoveAllComponent();

		virtual bool Update(float frameTime);

	private:
		Entity(const Entity&);
		Entity& operator=(const Entity&);

	private:
		TEntityUID m_UID;
		std::string m_Name;

		std::map<TEntityUID, EntityComponent*> m_Components;
	};
}

