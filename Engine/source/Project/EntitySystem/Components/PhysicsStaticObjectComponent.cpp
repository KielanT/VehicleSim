#include "ppch.h"
#include "PhysicsStaticObjectComponent.h"
#include "Vehicle/ComponentVehicleHelpers.h"

namespace Project
{
	bool PhysicsStaticObjectComponent::Update(float frameTime)
	{
		UpdatePositionAndRotation();
		return true;
	}
	
	void PhysicsStaticObjectComponent::UpdatePositionAndRotation()
	{
		if (m_RigidStatic != nullptr && m_ObjectType != PhysicsStaticObjectType::Plane)
		{
			CVector3 pos;
			pos.x = m_RigidStatic->getGlobalPose().p.x;
			pos.y = m_RigidStatic->getGlobalPose().p.y;
			pos.z = m_RigidStatic->getGlobalPose().p.z;
			m_Transform->SetPosition(pos);
			CVector3 rot;
			float w = m_RigidStatic->getGlobalPose().q.w;
			rot.x = m_RigidStatic->getGlobalPose().q.x;
			rot.y = m_RigidStatic->getGlobalPose().q.y;
			rot.z = m_RigidStatic->getGlobalPose().q.z;
			m_Transform->SetRotationFromQuat(rot, w);
		}
	}
	
	physx::PxShape* PhysicsStaticObjectComponent::GetPlane()
	{
		physx::PxShape* shapes[1];
		m_RigidStatic->getShapes(shapes, 1);
		physx::PxFilterData qryFilterData;
		DrivableSurface(qryFilterData);
		shapes[0]->setQueryFilterData(qryFilterData);

		physx::PxFilterData GroundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
		shapes[0]->setSimulationFilterData(GroundPlaneSimFilterData);
		
		return shapes[0];
	}
}