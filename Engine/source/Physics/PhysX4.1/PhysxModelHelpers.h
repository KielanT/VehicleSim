#pragma once
#include <PxPhysicsAPI.h>



namespace Project
{
	physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics* physics, physx::PxCooking* cooking);
	physx::PxTriangleMesh* CreateTriangleMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, const physx::PxU32 numTri, std::vector<unsigned int> indices32, physx::PxPhysics& physics, physx::PxCooking& cooking);
}