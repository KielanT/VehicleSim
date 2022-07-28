#include "ppch.h"
#include "PhysicsStaticObjectComponent.h"
#include "Vehicle/ComponentVehicleHelpers.h"

namespace Project
{
	PhysicsStaticObjectComponent::~PhysicsStaticObjectComponent()
	{
	}
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
	
	physx::PxShape* PhysicsStaticObjectComponent::GetBoxShape(bool isTrigger)
	{
		if (!isTrigger)
		{
			return physx::PxRigidActorExt::createExclusiveShape(*m_RigidStatic, physx::PxBoxGeometry(m_CollisionScale.x, m_CollisionScale.y, m_CollisionScale.z), *m_Material);
		}
		else
		{
			physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*m_RigidTriggerStatic, physx::PxBoxGeometry(m_TriggerScale.x, m_TriggerScale.y, m_TriggerScale.z), *m_Material);
			shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
			shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			return shape;
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
	physx::PxShape* PhysicsStaticObjectComponent::GetConvexMesh()
	{
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
			return shape = physx::PxRigidActorExt::createExclusiveShape(*m_RigidStatic, geom, *m_Material);
		}
		return shape;
	}
	physx::PxShape* PhysicsStaticObjectComponent::GetTriangleMesh()
	{
		physx::PxU32 vertexCount;
		std::vector<physx::PxVec3> vertices;

		if (m_Entity != nullptr)
		{
			if (m_Entity->GetComponent("Renderer"))
			{
				RendererComponent* comp = static_cast<RendererComponent*>(m_Entity->GetComponent("Renderer"));
				vertexCount = comp->GetNumberOfVertices(0);
				int triCount = comp->GetNumberTriangles();

				std::vector<CVector3> trackVertices = comp->GetVertices();
				for (int i = 0; i < vertexCount; i++)
				{
					vertices.push_back(physx::PxVec3(trackVertices[i].x, trackVertices[i].y, trackVertices[i].z));
				}
				physx::PxVec3* v = vertices.data();

				physx::PxU32* i = comp->GetIndices().data();

				physx::PxTriangleMeshGeometry geom = CreateTriangleMesh(v, vertexCount, triCount, comp->GetIndices(), *m_Physics->GetPhysics(), *m_Physics->GetCooking());


				if (m_IsDrivable == true)
				{
					physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*m_RigidStatic, geom, *m_Material);;
					physx::PxFilterData qryFilterData;
					DrivableSurface(qryFilterData);
					shape->setQueryFilterData(qryFilterData);

					physx::PxFilterData GroundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
					shape->setSimulationFilterData(GroundPlaneSimFilterData);
					return shape;
				}
				else
				{
					return physx::PxRigidActorExt::createExclusiveShape(*m_RigidStatic, geom, *m_Material);
				}

				
			}
		}
		return nullptr;
	}
	physx::PxShape* PhysicsStaticObjectComponent::GetSphere()
	{
		return physx::PxRigidActorExt::createExclusiveShape(*m_RigidStatic, physx::PxSphereGeometry(m_CollisionScale.x), *m_Material);;
	}
}