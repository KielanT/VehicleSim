#pragma once

// From physx snippets
#include <PxPhysicsAPI.h>

enum
{
	DRIVABLE_SURFACE = 0xffff0000,
	UNDRIVABLE_SURFACE = 0x0000ffff
};

physx::PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking
(physx::PxFilterData filterData0, physx::PxFilterData filterData1,
	const void* constantBlock, physx::PxU32 constantBlockSize,
	physx::PxHitFlags& queryFlags);

physx::PxQueryHitType::Enum WheelSceneQueryPostFilterBlocking
(physx::PxFilterData queryFilterData, physx::PxFilterData objectFilterData,
	const void* constantBlock, physx::PxU32 constantBlockSize,
	const physx::PxQueryHit& hit);

physx::PxQueryHitType::Enum WheelSceneQueryPreFilterNonBlocking
(physx::PxFilterData filterData0, physx::PxFilterData filterData1,
	const void* constantBlock, physx::PxU32 constantBlockSize,
	physx::PxHitFlags& queryFlags);

physx::PxQueryHitType::Enum WheelSceneQueryPostFilterNonBlocking
(physx::PxFilterData queryFilterData, physx::PxFilterData objectFilterData,
	const void* constantBlock, physx::PxU32 constantBlockSize,
	const physx::PxQueryHit& hit);

class VehicleSceneQueryData
{
public:
	VehicleSceneQueryData();
	~VehicleSceneQueryData();

	//Allocate scene query data for up to maxNumVehicles and up to maxNumWheelsPerVehicle with numVehiclesInBatch per batch query.
	static VehicleSceneQueryData* allocate
	(const physx::PxU32 maxNumVehicles, const physx::PxU32 maxNumWheelsPerVehicle, const physx::PxU32 maxNumHitPointsPerWheel, const physx::PxU32 numVehiclesInBatch,
		physx::PxBatchQueryPreFilterShader preFilterShader, physx::PxBatchQueryPostFilterShader postFilterShader,
		physx::PxAllocatorCallback& allocator);

	//Free allocated buffers.
	void free(physx::PxAllocatorCallback& allocator);

	//Create a PxBatchQuery instance that will be used for a single specified batch.
	static physx::PxBatchQuery* setUpBatchedSceneQuery(const physx::PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, physx::PxScene* scene);

	//Return an array of scene query results for a single specified batch.
	physx::PxRaycastQueryResult* getRaycastQueryResultBuffer(const physx::PxU32 batchId);

	//Return an array of scene query results for a single specified batch.
	physx::PxSweepQueryResult* getSweepQueryResultBuffer(const physx::PxU32 batchId);

	//Get the number of scene query results that have been allocated for a single batch.
	physx::PxU32 getQueryResultBufferSize() const;

private:

	//Number of queries per batch
	physx::PxU32 mNumQueriesPerBatch;

	//Number of hit results per query
	physx::PxU32 mNumHitResultsPerQuery;

	//One result for each wheel.
	physx::PxRaycastQueryResult* mRaycastResults;
	physx::PxSweepQueryResult* mSweepResults;

	//One hit for each wheel.
	physx::PxRaycastHit* mRaycastHitBuffer;
	physx::PxSweepHit* mSweepHitBuffer;

	//Filter shader used to filter drivable and non-drivable surfaces
	physx::PxBatchQueryPreFilterShader mPreFilterShader;

	//Filter shader used to reject hit shapes that initially overlap sweeps.
	physx::PxBatchQueryPostFilterShader mPostFilterShader;
};

