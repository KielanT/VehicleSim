#include "ppch.h"
#include "PhysicsDynamicObjectComponent.h"
#include "Physics/PhysX4.1/PhysxModelHelpers.h"

namespace Project
{
	PhysicsDynamicObjectComponent::~PhysicsDynamicObjectComponent()
	{
	}
	bool PhysicsDynamicObjectComponent::Update(float frameTime)
	{
		UpdatePositionAndRotation(); // Updates the position for any physic simulation
		return true;
	}
	void PhysicsDynamicObjectComponent::UpdatePositionAndRotation()
	{
		// Only update position and rotation if the actor exists
		if (m_RigidDynamic != nullptr)
		{
			CVector3 pos;
			pos.x = m_RigidDynamic->getGlobalPose().p.x;
			pos.y = m_RigidDynamic->getGlobalPose().p.y;
			pos.z = m_RigidDynamic->getGlobalPose().p.z;
			m_Transform->SetPosition(pos);
			CVector3 rot;
			float w = m_RigidDynamic->getGlobalPose().q.w;
			rot.x = m_RigidDynamic->getGlobalPose().q.x;
			rot.y = m_RigidDynamic->getGlobalPose().q.y;
			rot.z = m_RigidDynamic->getGlobalPose().q.z;
			m_Transform->SetRotationFromQuat(rot, w);
		}
	}
	
	physx::PxShape* PhysicsDynamicObjectComponent::GetBoxShape() 
	{
		return physx::PxRigidActorExt::createExclusiveShape(*m_RigidDynamic, physx::PxBoxGeometry(m_CollisionScale.x, m_CollisionScale.y, m_CollisionScale.z), *m_Material);;
	}
	physx::PxShape* PhysicsDynamicObjectComponent::GetSphereShape()
	{
		return physx::PxRigidActorExt::createExclusiveShape(*m_RigidDynamic, physx::PxSphereGeometry(m_CollisionScale.x), *m_Material);
	}

	physx::PxShape* PhysicsDynamicObjectComponent::GetConvextMeshShape()
	{
		// Creates convex dynamic
		physx::PxShape* shape = nullptr;
		if (m_Entity->GetComponent("Renderer"))
		{
			RendererComponent* comp = static_cast<RendererComponent*>(m_Entity->GetComponent("Renderer"));
			physx::PxU32 vertexCount;
			std::vector<physx::PxVec3> vertices;
			vertexCount = comp->GetNumberOfVertices(0);

			std::vector<CVector3> meshVertice = comp->GetVertices();

			for (int i = 0; i < vertexCount; ++i)
			{
				vertices.push_back({ meshVertice[i].x,  meshVertice[i].y, meshVertice[i].z });
			}

			physx::PxVec3* v = vertices.data();

			physx::PxConvexMeshGeometry geom = CreateConvexMesh(v, vertexCount, m_Physics->GetPhysics(), m_Physics->GetCooking());
			return shape = physx::PxRigidActorExt::createExclusiveShape(*m_RigidDynamic, geom, *m_Material);
		}
		return shape;
	}

}