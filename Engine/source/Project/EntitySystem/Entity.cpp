#include "ppch.h"
#include "Entity.h"
#include "Project/EntitySystem/Components/TransformComponent.h"

namespace Project
{

	Entity::Entity(TEntityUID UID, const std::string& name)
	{
		m_UID = UID;
		m_Name = name;
	}

	void Entity::AddComponent(EntityComponent* component)
	{
		m_Components.insert(std::pair<TEntityUID, EntityComponent*>(component->GetUID(), component));
	}

	EntityComponent* Entity::GetComponent(TEntityUID UID)
	{
		std::map<TEntityUID, EntityComponent*>::iterator it = m_Components.find(UID);
		if (it != m_Components.end())
		{
			return (*it).second;
		}
		return nullptr;
	}

	EntityComponent* Entity::GetComponent(const std::string& type)
	{
		std::map<TEntityUID, EntityComponent*>::iterator it = m_Components.begin();
		while (it != m_Components.end())
		{
			if ((*it).second->GetType() == type)
			{
				return (*it).second;
			}
			it++;
		}
		return nullptr;
	}

	bool Entity::RemoveComponent(TEntityUID UID)
	{
		std::map<TEntityUID, EntityComponent*>::iterator it = m_Components.find(UID);
		if (it != m_Components.end())
		{
			delete (*it).second;
			m_Components.erase(it);
			return true;
		}
		return false;
	}

	bool Entity::RemoveComponent(const std::string& type)
	{
		std::map<TEntityUID, EntityComponent*>::iterator it = m_Components.begin();
		while (it != m_Components.end())
		{
			if ((*it).second->GetType() == type)
			{
				delete (*it).second;
				m_Components.erase(it);
				return true;
			}
			it++;
		}
		return false;
	}

	void Entity::RemoveAllComponent()
	{
		std::map<TEntityUID, EntityComponent*>::iterator it = m_Components.begin();
		while (it != m_Components.end())
		{
			if ((*it).second != nullptr)
			{
				delete (*it).second;
			}
			it++;
		}
		m_Components.clear();
	}

	bool Entity::Update(float frameTime)
	{
		// While loop for recieving messages

		std::map<TEntityUID, EntityComponent*>::iterator it = m_Components.begin();
		while (it != m_Components.end())
		{
			if (!(*it).second->Update(frameTime))
			{
				it = m_Components.erase(it);
			}
			else
			{
				it++;
			}
		}

		return true;
	}
}