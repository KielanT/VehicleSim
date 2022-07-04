#include "ppch.h"
#include "PhysxModelHelpers.h"


namespace Project
{
	physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics* physics, physx::PxCooking* cooking)
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
	physx::PxTriangleMesh* CreateTriangleMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, const physx::PxU32 numTri, std::vector<unsigned int> indices32, physx::PxPhysics& physics, physx::PxCooking& cooking)
	{
		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = numVerts;
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.points.data = verts;

		meshDesc.triangles.count = numTri;
		meshDesc.triangles.stride = 3 * sizeof(unsigned int);
		meshDesc.triangles.data = &indices32[0];


		physx::PxDefaultMemoryOutputStream writeBuffer;
		bool status = cooking.cookTriangleMesh(meshDesc, writeBuffer);
		if (!status)
			return nullptr;


		physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		return physics.createTriangleMesh(readBuffer);

//		physx::PxTolerancesScale scale;
//		physx::PxCookingParams params(scale);
//		// disable mesh cleaning - perform mesh validation on development configurations
//		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
//		// disable edge precompute, edges are set for each triangle, slows contact generation
//		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
//		// lower hierarchy for internal mesh
//		//params.meshCookingHint = physx::PxMeshCookingHint::eCOOKING_PERFORMANCE;
//
//		cooking.setParams(params);
//
//		physx::PxTriangleMeshDesc meshDesc;
//		meshDesc.points.count = numVerts;
//		meshDesc.points.stride = sizeof(physx::PxVec3);
//		meshDesc.points.data = verts;
//
//		meshDesc.triangles.count = numTri;
//		meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
//		meshDesc.triangles.data = indices32;
//
//#ifdef _DEBUG
//		// mesh should be validated before cooked without the mesh cleaning
//		bool res = cooking.validateTriangleMesh(meshDesc);
//		PX_ASSERT(res);
//#endif
//
//		return cooking.createTriangleMesh(meshDesc,
//			physics.getPhysicsInsertionCallback());
	}

	

}