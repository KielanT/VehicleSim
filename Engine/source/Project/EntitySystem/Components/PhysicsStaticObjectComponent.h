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
		//Sphere,
		ConvexMesh,
		//TriangleMesh,
		//HeightField
	};

	
	class PhysicsStaticObjectComponent : public EntityComponent
	{
	public:
		PhysicsStaticObjectComponent(Entity* entity, TEntityUID UID, PhysicsStaticObjectType objectType, IPhysics* physics, bool isDrivable = false, CVector3 colScale = { 1.0f, 1.0f, 1.0f }) : EntityComponent("PhysicsStaticObject", UID, entity)
		{
			m_Entity = entity;
			m_ObjectType = objectType;
			m_Physics = physics;
			m_IsDrivable = isDrivable;
			m_CollisionScale = colScale;

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
				m_Shape = GetBoxShape();
				break;
			case(PhysicsStaticObjectType::Plane):
				m_Shape = GetPlane();
				break;
			case(PhysicsStaticObjectType::ConvexMesh):
				m_Shape = GetConvexMesh();
				break;
			}

			UpdatePositionAndRotation();
			m_Physics->GetScene()->addActor(*m_RigidStatic);
		}

		virtual bool Update(float frameTime) override;
		void UpdatePositionAndRotation();

	private:
		physx::PxShape* GetBoxShape();
		physx::PxShape* GetPlane();
		physx::PxShape* GetConvexMesh();
		
	private:
		Entity* m_Entity;
		IPhysics* m_Physics;
		PhysicsStaticObjectType m_ObjectType;
		physx::PxMaterial* m_Material;

		physx::PxShape* m_Shape;
		physx::PxRigidStatic* m_RigidStatic;

		bool m_IsDrivable;

		TransformComponent* m_Transform;
		CVector3 m_CollisionScale;
	};
}
