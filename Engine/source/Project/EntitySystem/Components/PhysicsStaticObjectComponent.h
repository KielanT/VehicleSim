#pragma once
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/Interfaces/IPhysics.h"
#include "TransformComponent.h"

namespace Project
{
	enum class PhysicsStaticObjectType
	{
		Box = 0,
		Plane,
		//Capsule,
		Sphere,
		ConvexMesh,
		TriangleMesh,
		//HeightField
	};

	
	class PhysicsStaticObjectComponent : public EntityComponent
	{
	public:
		PhysicsStaticObjectComponent(Entity* entity, TEntityUID UID, PhysicsStaticObjectType objectType, IPhysics* physics, bool isDrivable = false, CVector3 colScale = { 1.0f, 1.0f, 1.0f }, bool isTrigger = false,  CVector3 trigScale = { 1.0f, 1.0f, 1.0f }, CVector3 trigPos = { 0.0f, 0.0f, 0.0f }, PhysicsStaticObjectType triggerOjectType = PhysicsStaticObjectType::Box) : EntityComponent("PhysicsStaticObject", UID, entity)
		{
			m_Entity = entity;
			m_ObjectType = objectType;
			m_Physics = physics;
			m_IsDrivable = isDrivable;
			m_CollisionScale = colScale;
			m_IsTrigger = isTrigger;
			m_TriggerObjectType = triggerOjectType;
			m_TriggerScale = trigScale;

			if (m_Entity->GetComponent("Transform"))
			{
				m_Transform = static_cast<TransformComponent*>(m_Entity->GetComponent("Transform"));
			}

			m_Material = m_Physics->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);

			if(m_ObjectType != PhysicsStaticObjectType::Plane)
				m_RigidStatic = m_Physics->GetPhysics()->createRigidStatic(physx::PxTransform({ m_Transform->GetPosition().x ,  m_Transform->GetPosition().y ,  m_Transform->GetPosition().z }));
			else
				m_RigidStatic = physx::PxCreatePlane(*m_Physics->GetPhysics(), physx::PxPlane(0, 1, 0, 0), *m_Material);
			
			switch (m_ObjectType)
			{
			case (PhysicsStaticObjectType::Box):
				m_Shape = GetBoxShape(false);
				break;
			case(PhysicsStaticObjectType::Plane):
				m_Shape = GetPlane();
				break;
			case(PhysicsStaticObjectType::Sphere):
				m_Shape = GetSphere();
				break;
			case(PhysicsStaticObjectType::ConvexMesh):
				m_Shape = GetConvexMesh();
				break;
			case(PhysicsStaticObjectType::TriangleMesh):
				m_Shape = GetTriangleMesh();
				break;
			}

			if (m_IsTrigger)
			{
				if (m_TriggerObjectType == PhysicsStaticObjectType::Box)
				{

					
					m_RigidTriggerStatic = m_Physics->GetPhysics()->createRigidStatic({ m_Transform->GetPosition().x + trigPos.x,  m_Transform->GetPosition().y + trigPos.y , m_Transform->GetPosition().z + trigPos.z });
					if (m_RigidTriggerStatic != nullptr)
					{
						m_TriggerShape = GetBoxShape(true);
					}
					m_Physics->GetScene()->addActor(*m_RigidTriggerStatic);

				}
			}

			UpdatePositionAndRotation();
			m_Physics->GetScene()->addActor(*m_RigidStatic);
		}

		~PhysicsStaticObjectComponent();

		virtual bool Update(float frameTime) override;
		void UpdatePositionAndRotation();
		physx::PxActor* GetTriggerActor() 
		{ 
			return m_RigidTriggerStatic;
		}

	private:
		physx::PxShape* GetBoxShape(bool isTrigger);
		physx::PxShape* GetPlane();
		physx::PxShape* GetConvexMesh();
		physx::PxShape* GetTriangleMesh();
		physx::PxShape* GetSphere();
		
	private:
		Entity* m_Entity;
		IPhysics* m_Physics;
		PhysicsStaticObjectType m_ObjectType;
		physx::PxMaterial* m_Material;

		physx::PxShape* m_Shape = nullptr;
		physx::PxRigidStatic* m_RigidStatic = nullptr;

		physx::PxShape* m_TriggerShape = nullptr;
		physx::PxRigidStatic* m_RigidTriggerStatic = nullptr;
		PhysicsStaticObjectType m_TriggerObjectType;

		bool m_IsDrivable;
		bool m_IsTrigger;

		TransformComponent* m_Transform;
		CVector3 m_CollisionScale;
		CVector3 m_TriggerScale;
	};
}
