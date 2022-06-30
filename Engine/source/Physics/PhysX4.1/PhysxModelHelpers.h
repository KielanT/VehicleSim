#pragma once
#include <PxPhysicsAPI.h>

namespace Project
{
	static physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics* physics, physx::PxCooking* cooking)
	{
		// Create descriptor for convex mesh
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = numVerts;
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = verts;
		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxConvexMesh* convexMesh = NULL;
		physx::PxDefaultMemoryOutputStream buf;
		if (cooking->cookConvexMesh(convexDesc, buf))
		{
			physx::PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
			convexMesh = physics->createConvexMesh(id);
		}
		

		return convexMesh;
	}
}