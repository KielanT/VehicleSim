#pragma once
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/Interfaces/IPhysics.h"
#include "TransformComponent.h"

namespace Project
{
	// The implmented dynamic objects
	enum class PhysicsDynmaicObjectType
	{
		Box = 0,
		//Plane,
		//Capsule,
		Sphere,
		ConvexMesh,
		//TriangleMesh,
		//HeightField
	};

	class PhysicsDynamicObjectComponent : public EntityComponent
	{
	public:
		PhysicsDynamicObjectComponent(Entity* entity, TEntityUID UID, PhysicsDynmaicObjectType objectType, std::shared_ptr<IPhysics> physics, bool isDrivable = false, CVector3 colScale = { 1.0f, 1.0f, 1.0f }) : EntityComponent("PhysicsDynamicObject", UID, entity)
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

			m_RigidDynamic = m_Physics->GetPhysics()->createRigidDynamic(physx::PxTransform({ m_Transform->GetPosition().x ,  m_Transform->GetPosition().y ,  m_Transform->GetPosition().z }));

			switch (m_ObjectType)
			{
			case (PhysicsDynmaicObjectType::Box):
				m_Shape = GetBoxShape();
				break;
			case(PhysicsDynmaicObjectType::Sphere):
				m_Shape = GetSphereShape();
				break;
			/*case (PhysicsDynmaicObjectType::Plane):
				m_Shape = GetPlane();
				
				break;*/
			case(PhysicsDynmaicObjectType::ConvexMesh):
				m_Shape = GetConvextMeshShape();
				break;
			}

			UpdatePositionAndRotation();
			m_Physics->GetScene()->addActor(*m_RigidDynamic);
		}
		
		~PhysicsDynamicObjectComponent();

		virtual bool Update(float frameTime) override;
		void UpdatePositionAndRotation();

		physx::PxRigidDynamic* GetActor() { return m_RigidDynamic; }
		
	private:
		// Creates the shapes required
		physx::PxShape* GetBoxShape();
		physx::PxShape* GetSphereShape();
		physx::PxShape* GetConvextMeshShape();


	private:

		// Member variables
		Entity* m_Entity;
		std::shared_ptr<IPhysics> m_Physics;
		PhysicsDynmaicObjectType m_ObjectType;
		physx::PxMaterial* m_Material;

		physx::PxShape* m_Shape;
		physx::PxRigidDynamic* m_RigidDynamic;
		
		bool m_IsDrivable;

		TransformComponent* m_Transform;
		CVector3 m_CollisionScale;
		
	};
}
