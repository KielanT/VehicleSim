#pragma once
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/Interfaces/IPhysics.h"
#include "TransformComponent.h"

namespace Project
{
    enum class PhysicsObjectType
    {
		Box = 0,
		//Plane,
		//Capsule,
		//Sphere,
		//ConvexMesh,
		//TriangleMesh,
		//HeightField
    };
	
    enum class RigidBodyType
    {
        Dynamic = 0,
        //Static
    };
	
    class PhysicsObjectComponent : public EntityComponent
    {
    public:
        PhysicsObjectComponent(Entity* entity, TEntityUID UID, PhysicsObjectType objectType, RigidBodyType rigidType, IPhysics* physics, CVector3 colScale = {1.0f, 1.0f, 1.0f}) : EntityComponent("PhysicsObject", UID, entity)
        {
            m_Entity = entity;
            m_ObjectType = objectType;
            m_RigidType = rigidType;
            m_Physics = physics;
            m_CollisionScale = colScale;
			
            if (m_Entity->GetComponent("Transform"))
            {
                m_Transform = static_cast<TransformComponent*>(m_Entity->GetComponent("Transform"));
            }
			
            m_Material = m_Physics->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
			
            switch (m_RigidType)
            {
            case(RigidBodyType::Dynamic):
                m_RigidDynamic = m_Physics->GetPhysics()->createRigidDynamic(physx::PxTransform({ m_Transform->GetPosition().x ,  m_Transform->GetPosition().y ,  m_Transform->GetPosition().z }));
                break;
            }
			
            switch (m_ObjectType)
            {
            case (PhysicsObjectType::Box):
                m_Shape = GetBoxShape();
                break;
          /*  case (PhysicsObjectType::Plane):
                m_Shape = GetPlaneShape();
                break;*/
            }

          

            UpdatePositionAndRotation();
            m_Physics->GetScene()->addActor(*m_RigidDynamic);
        }
        virtual bool Update(float frameTime) override;
        physx::PxRigidDynamic* GetActor() {   return m_RigidDynamic; }
        void UpdatePositionAndRotation();
    private:
        physx::PxShape* GetBoxShape();
       // physx::PxShape* GetPlaneShape();
		
       

        
		
    private:
        Entity* m_Entity;
        IPhysics* m_Physics;
		
        physx::PxMaterial* m_Material;

        PhysicsObjectType m_ObjectType;
        RigidBodyType m_RigidType;
		
        physx::PxShape* m_Shape;
        physx::PxRigidDynamic* m_RigidDynamic = nullptr;
        physx::PxRigidStatic* m_RigidStatic = nullptr;
		
        TransformComponent* m_Transform;
        CVector3 m_CollisionScale;
    };
}
