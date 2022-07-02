#include "ppch.h"
#include "PhysicsDynamicObjectComponent.h"

namespace Project
{
	bool PhysicsDynamicObjectComponent::Update(float frameTime)
	{
		UpdatePositionAndRotation();
		return true;
	}
	void PhysicsDynamicObjectComponent::UpdatePositionAndRotation()
	{
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

}