#include "ppch.h"
#include "ComponentVehicleHelpers.h"


namespace Project
{


physx::PxConvexMesh* CreateWheelMesh(int index, Entity* entity, IPhysics* physics)
{
	physx::PxU32 vertexCount;
	std::vector<physx::PxVec3> vertices;

	if (entity->GetComponent("CollisionMesh"))
	{
		CollisionComponent* comp = static_cast<CollisionComponent*>(entity->GetComponent("CollisionMesh"));
		vertexCount = comp->GetNumberOfVertices(index);

		std::vector<CVector3> Wheels = comp->GetVertices(index);

		for (int i = 0; i < vertexCount; ++i)
		{
			vertices.push_back({ Wheels[i].x,  Wheels[i].y, Wheels[i].z });
		}
	}

	physx::PxVec3* v = vertices.data();

	return CreateConvexMesh(v, vertexCount, physics->GetPhysics(), physics->GetCooking());
}

physx::PxConvexMesh* CreateChassisMesh(int index, Entity* entity, IPhysics* physics)
{
	physx::PxU32 vertexCount;
	std::vector<physx::PxVec3> vertices;
	if (entity->GetComponent("CollisionMesh"))
	{
		CollisionComponent* comp = static_cast<CollisionComponent*>(entity->GetComponent("CollisionMesh"));
		vertexCount = comp->GetNumberOfVertices(index);

		std::vector<CVector3> Chassis = comp->GetVertices(index);

		for (int i = 0; i < vertexCount; ++i)
		{
			vertices.push_back({ Chassis[i].x,  Chassis[i].y, Chassis[i].z });
		}
	}

	physx::PxVec3* v = vertices.data();

	return CreateConvexMesh(v, vertexCount, physics->GetPhysics(), physics->GetCooking());
}

void MakeWheelWidthsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii)
{
	for (physx::PxU32 i = 0; i < 4; i++)
	{
		const physx::PxU32 numWheelVerts = wheelConvexMeshes[i]->getNbVertices();
		const physx::PxVec3* wheelVerts = wheelConvexMeshes[i]->getVertices();
		physx::PxVec3 wheelMin(PX_MAX_F32, PX_MAX_F32, PX_MAX_F32);
		physx::PxVec3 wheelMax(-PX_MAX_F32, -PX_MAX_F32, -PX_MAX_F32);
		for (physx::PxU32 j = 0; j < numWheelVerts; j++)
		{
			wheelMin.x = physx::PxMin(wheelMin.x, wheelVerts[j].x);
			wheelMin.y = physx::PxMin(wheelMin.y, wheelVerts[j].y);
			wheelMin.z = physx::PxMin(wheelMin.z, wheelVerts[j].z);
			wheelMax.x = physx::PxMax(wheelMax.x, wheelVerts[j].x);
			wheelMax.y = physx::PxMax(wheelMax.y, wheelVerts[j].y);
			wheelMax.z = physx::PxMax(wheelMax.z, wheelVerts[j].z);
		}
		wheelWidths[i] = wheelMax.x - wheelMin.x;
		wheelRadii[i] = physx::PxMax(wheelMax.y, wheelMax.z) * 0.975f;
	}
}

physx::PxVec3 MakeChassis(physx::PxConvexMesh* chassisConvexMesh)
{
	const physx::PxU32 numChassisVerts = chassisConvexMesh->getNbVertices();
	const physx::PxVec3* chassisVerts = chassisConvexMesh->getVertices();
	physx::PxVec3 chassisMin(PX_MAX_F32, PX_MAX_F32, PX_MAX_F32);
	physx::PxVec3 chassisMax(-PX_MAX_F32, -PX_MAX_F32, -PX_MAX_F32);
	for (physx::PxU32 i = 0; i < numChassisVerts; i++)
	{
		chassisMin.x = physx::PxMin(chassisMin.x, chassisVerts[i].x);
		chassisMin.y = physx::PxMin(chassisMin.y, chassisVerts[i].y);
		chassisMin.z = physx::PxMin(chassisMin.z, chassisVerts[i].z);
		chassisMax.x = physx::PxMax(chassisMax.x, chassisVerts[i].x);
		chassisMax.y = physx::PxMax(chassisMax.y, chassisVerts[i].y);
		chassisMax.z = physx::PxMax(chassisMax.z, chassisVerts[i].z);
	}

	const physx::PxVec3 chassisDims = chassisMax - chassisMin;
	return chassisDims;
}

void ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32* wheelWidth, const physx::PxF32* wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets)
{
	const physx::PxF32 numLeftWheels = numWheels / 2.0f;
	const physx::PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f);

	wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth[2]) * 0.5f, -0.3f, wheelRearZ + 0 * deltaZ * 0.5f);
	wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth[3]) * 0.5f, -0.3f, wheelRearZ + 0 * deltaZ * 0.5f);
	wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth[0]) * 0.5f, -0.3f, wheelRearZ + (numLeftWheels - 1) * deltaZ);
	wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth[1]) * 0.5f, -0.3f, wheelRearZ + (numLeftWheels - 1) * deltaZ);

}

static physx::PxF32 gTireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
{
	//NORMAL,	WORN
	{1.00f,		0.1f}//TARMAC
};

physx::PxVehicleDrivableSurfaceToTireFrictionPairs* CreateFrictionPairs(const physx::PxMaterial* defaultMaterial)
{
	physx::PxVehicleDrivableSurfaceType surfaceTypes[1];
	surfaceTypes[0].mType = SURFACE_TYPE_TARMAC;

	const physx::PxMaterial* surfaceMaterials[1];
	surfaceMaterials[0] = defaultMaterial;

	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
		physx::PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES);

	surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES, surfaceMaterials, surfaceTypes);

	for (physx::PxU32 i = 0; i < MAX_NUM_SURFACE_TYPES; i++)
	{
		for (physx::PxU32 j = 0; j < MAX_NUM_TIRE_TYPES; j++)
		{
			surfaceTirePairs->setTypePairFriction(i, j, gTireFrictionMultipliers[i][j]);
		}
	}
	return surfaceTirePairs;
}

}