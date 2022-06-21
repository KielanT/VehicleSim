#include "ppch.h"
#include "PhysicsObjectComponent.h"
#include "ComponentVehicleHelpers.h"

namespace Project
{
	bool PhysicsObjectComponent::Update(float frameTime)
	{
		UpdatePositionAndRotation();
		return true;
	}
	physx::PxShape* PhysicsObjectComponent::GetBoxShape()
	{
		physx::PxShape* shape = nullptr;
		if (m_RigidType == RigidBodyType::Dynamic)
		{
			
			shape = physx::PxRigidActorExt::createExclusiveShape(*m_RigidDynamic, physx::PxBoxGeometry(m_CollisionScale.x, m_CollisionScale.y, m_CollisionScale.z), *m_Material);
		}

		
		return shape;
	}
	physx::PxRigidStatic* PhysicsObjectComponent::CreatePlane()
	{
		physx::PxFilterData GroundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
		//Add a plane to the scene.
		physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*m_Physics->GetPhysics(), physx::PxPlane(0, 1, 0, 0), *m_Material);

		//Get the plane shape so we can set query and simulation filter data.
		physx::PxShape* shapes[1];
		groundPlane->getShapes(shapes, 1);

		//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
		physx::PxFilterData qryFilterData;
		DrivableSurface(qryFilterData);
		shapes[0]->setQueryFilterData(qryFilterData);

		//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
		shapes[0]->setSimulationFilterData(GroundPlaneSimFilterData);

		return groundPlane;
	}
	void PhysicsObjectComponent::UpdatePositionAndRotation()
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
}