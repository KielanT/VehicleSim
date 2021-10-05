#pragma once
#include "Project/EntitySystem/EntityManager.h"
#include "Math/CVector3.h"
#include <tinyxml2.h>

// Implemented using Game dev 2: game data lab

namespace Project
{
	class CParseLevel
	{
	public:
		// Constructor just stores a pointer to the entity manager so all methods below can access it
		CParseLevel(EntityManager* entityManager) : m_EntityManager(entityManager) {}

		bool ParseFile(const std::string& fileName);

	private:
		bool ParseLevelElement(tinyxml2::XMLElement* rootElement);
		bool ParseEntitiesElement(tinyxml2::XMLElement* rootElement);
		bool ParseComponentElement(tinyxml2::XMLElement* rootElement, Entity* entity);

		CVector3 GetVector3FromElement(tinyxml2::XMLElement* rootElement);

		EPixelShader GetPixelShader(std::string value);
		EVertexShader GetVertexShader(std::string value);

		EBlendState GetBlendState(std::string value);
		EDepthStencilState GetDepthStencilState(std::string value);
		ERasterizerState GetRasterizerState(std::string value);
		ESamplerState GetSamplerState(std::string value);

	private:
		// Constructer is passed a pointer to an entity manager used to create 
		// entities as they are parsed
		EntityManager* m_EntityManager;
	};

}