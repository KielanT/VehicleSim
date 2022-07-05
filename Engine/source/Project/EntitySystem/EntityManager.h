#pragma once

#include "Project/Core.h"
#include "Utility/Lab/HashTable.h"
#include "Entity.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/Interfaces/IShader.h"
#include "Project/Interfaces/IState.h"
#include "Project/Interfaces/IPhysics.h"
#include "Graphics/DirectX11/CDirectX11Shader.h"
#include "Graphics/DirectX11/CDirectX11States.h"
#include "Math/CVector3.h"
#include "Components/VehicleComponent.h"
#include "Components/PhysicsDynamicObjectComponent.h" 
#include "Components/PhysicsStaticObjectComponent.h" 

namespace Project
{
	struct SEntityTransform
	{
		CVector3 Position;
		CVector3 Rotation;
		CVector3 Scale;

		SEntityTransform(CVector3 pos = CVector3(0.0f, 0.0f, 0.0f), CVector3 rot = CVector3(0.0f, 0.0f, 0.0f), CVector3 scale = CVector3(1.0f, 1.0f, 1.0f))
			: Position(pos), Rotation(rot), Scale(scale)
		{}
		SEntityTransform(float x, float y, float z) : Position({ x, y, z }), Rotation(CVector3(0.0f, 0.0f, 0.0f)), Scale(CVector3(1.0f, 1.0f, 1.0f))
		{
		}
		
	};

	class P_API EntityManager
	{
	public:
		EntityManager(IRenderer* renderer);
		EntityManager(IRenderer* renderer, IPhysics* physics);
		~EntityManager();

	public:
		TEntityUID CreateEntity(const std::string& name);
		TEntityUID CreateModelEntity(const std::string& name, std::string fileMeshPath, bool isRendered = true, std::string filePath = "media/BasicTexOrange.png",
			SEntityTransform transform = SEntityTransform(),
			EPixelShader pixelShader = EPixelShader::PixelLightingPixelShader,
			EVertexShader vertexShader = EVertexShader::PixelLightingVertexShader,
			EBlendState blendState = EBlendState::NoBlendingState,
			EDepthStencilState depthStencilState = EDepthStencilState::UseDepthBufferState,
			ERasterizerState rasterizerState = ERasterizerState::CullNoneState,
			ESamplerState samplerState = ESamplerState::Anisotropic4xSampler);

		TEntityUID CreateLightEntity(const std::string& name);

		TEntityUID CreateVehicleEntity(const std::string& name, std::string mainMeshPath, std::string collisionMeshPath,
			VehicleSettings vehicleSettings = VehicleSettings(), std::string texturePath = "media/BasicTexWhite.png",
			SEntityTransform transform = SEntityTransform(), EPixelShader pixelShader = EPixelShader::PixelLightingPixelShader,
			EVertexShader vertexShader = EVertexShader::PixelLightingVertexShader,
			EBlendState blendState = EBlendState::NoBlendingState,
			EDepthStencilState depthStencilState = EDepthStencilState::UseDepthBufferState,
			ERasterizerState rasterizerState = ERasterizerState::CullNoneState,
			ESamplerState samplerState = ESamplerState::Anisotropic4xSampler);
		
		TEntityUID CreatePhysicsDynamicEntity(const std::string& name, PhysicsDynmaicObjectType objectType, std::string mainMeshPath,
			SEntityTransform transform = SEntityTransform(), CVector3 colScale = { 1.0f, 1.0f, 1.0f }, bool isDrivable = false,
			std::string texturePath = "media/BasicTexWhite.png",
			EPixelShader pixelShader = EPixelShader::PixelLightingPixelShader,
			EVertexShader vertexShader = EVertexShader::PixelLightingVertexShader,
			EBlendState blendState = EBlendState::NoBlendingState,
			EDepthStencilState depthStencilState = EDepthStencilState::UseDepthBufferState,
			ERasterizerState rasterizerState = ERasterizerState::CullNoneState,
			ESamplerState samplerState = ESamplerState::Anisotropic4xSampler);
	

		TEntityUID CreatePhysicsStaticEntity(const std::string& name, PhysicsStaticObjectType objectType, std::string mainMeshPath, 
			SEntityTransform transform = SEntityTransform(), CVector3 colScale = { 1.0f, 1.0f, 1.0f }, bool isDrivable = false, std::string texturePath = "media/BasicTexWhite.png",
			bool isTrigger = false, SEntityTransform triggerTransform = SEntityTransform(), PhysicsStaticObjectType triggerObjectType = PhysicsStaticObjectType::Box,
			EPixelShader pixelShader = EPixelShader::PixelLightingPixelShader,
			EVertexShader vertexShader = EVertexShader::PixelLightingVertexShader,
			EBlendState blendState = EBlendState::NoBlendingState,
			EDepthStencilState depthStencilState = EDepthStencilState::UseDepthBufferState,
			ERasterizerState rasterizerState = ERasterizerState::CullNoneState,
			ESamplerState samplerState = ESamplerState::Anisotropic4xSampler);

		bool DestroyEntity(TEntityUID UID);

		void DestroyAllEntities();

		const TEntityUID GetNewUID() { return m_NextUID++; }

		const TInt32 NumEntities()
		{
			return static_cast<TInt32>(m_Entities.size());
		}

		const Entity* GetEntityAtIndex(TInt32 index)
		{
			return m_Entities[index];
		}

		Entity* GetEntity(TEntityUID UID)
		{
			TInt32 entityIndex;
			if (!m_EntityUIDMap->LookUpKey(UID, &entityIndex))
			{
				return nullptr;
			}
			return m_Entities[entityIndex];
		}

		// Return the entity with the given name & optionally the given template name & type
		Entity* GetEntity(const std::string& name)
		{
			TEntityIter entity = m_Entities.begin();
			while (entity != m_Entities.end())
			{
				if ((*entity)->GetName() == name)
				{
					return (*entity);
				}
				++entity;
			}
			return nullptr;
		}

		void BeginEnumEntities(const std::string& name)
		{
			m_IsEnumerating = true;
			m_EnumEntity = m_Entities.begin();
			m_EnumName = name;
		}

		void EndEnumEntities()
		{
			m_IsEnumerating = false;
		}

		const Entity* EnumEntity()
		{
			if (!m_IsEnumerating)
			{
				return nullptr;
			}

			while (m_EnumEntity != m_Entities.end())
			{
				if ((m_EnumName.length() == 0 || (*m_EnumEntity)->GetName() == m_EnumName))
				{
					Entity* foundEntity = *m_EnumEntity;
					++m_EnumEntity;
					return foundEntity;
				}
				++m_EnumEntity;
			}

			m_IsEnumerating = false;
			return nullptr;

		}

		void UpdateAllEntities(float updateTime);

		void RenderAllEntities();

		const IShader* GetShader() { return m_Shader; }

	private:
		EntityManager(const EntityManager&);
		EntityManager& operator=(const EntityManager&);

	private:
		IRenderer* m_Renderer;

		typedef std::vector<Entity*> TEntities;
		typedef TEntities::iterator TEntityIter;

		TEntities m_Entities;

		CHashTable<TEntityUID, TUInt32>* m_EntityUIDMap;

		TEntityUID m_NextUID;

		bool m_IsEnumerating;
		TEntityIter m_EnumEntity;
		std::string m_EnumName;

		IShader* m_Shader;
		IState* m_State;

		bool m_IsPhysics = false;
		IPhysics* m_Physics;
	};

}