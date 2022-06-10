#include "ppch.h"
#include "VehicleSceneQueryData.h"

physx::PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking(physx::PxFilterData filterData0, physx::PxFilterData filterData1, const void* constantBlock, physx::PxU32 constantBlockSize, physx::PxHitFlags& queryFlags)
{
	//filterData0 is the vehicle suspension query.
	//filterData1 is the shape potentially hit by the query.
	PX_UNUSED(filterData0);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(queryFlags);
	return ((0 == (filterData1.word3 & DRIVABLE_SURFACE)) ? physx::PxQueryHitType::eNONE : physx::PxQueryHitType::eBLOCK);
}

physx::PxQueryHitType::Enum WheelSceneQueryPostFilterBlocking
(physx::PxFilterData filterData0, physx::PxFilterData filterData1,
	const void* constantBlock, physx::PxU32 constantBlockSize,
	const physx::PxQueryHit& hit)
{
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	if ((static_cast<const physx::PxSweepHit&>(hit)).hadInitialOverlap())
		return physx::PxQueryHitType::eNONE;
	return physx::PxQueryHitType::eBLOCK;
}

physx::PxQueryHitType::Enum WheelSceneQueryPreFilterNonBlocking(physx::PxFilterData filterData0, physx::PxFilterData filterData1, const void* constantBlock, physx::PxU32 constantBlockSize, physx::PxHitFlags& queryFlags)
{
	//filterData0 is the vehicle suspension query.
	//filterData1 is the shape potentially hit by the query.
	PX_UNUSED(filterData0);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(queryFlags);
	return ((0 == (filterData1.word3 & DRIVABLE_SURFACE)) ? physx::PxQueryHitType::eNONE : physx::PxQueryHitType::eTOUCH);
}

physx::PxQueryHitType::Enum WheelSceneQueryPostFilterNonBlocking
(physx::PxFilterData filterData0, physx::PxFilterData filterData1,
	const void* constantBlock, physx::PxU32 constantBlockSize,
	const physx::PxQueryHit& hit)
{
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);
	if ((static_cast<const physx::PxSweepHit&>(hit)).hadInitialOverlap())
		return physx::PxQueryHitType::eNONE;
	return physx::PxQueryHitType::eTOUCH;
}

VehicleSceneQueryData::VehicleSceneQueryData()
	: mNumQueriesPerBatch(0),
	mNumHitResultsPerQuery(0),
	mRaycastResults(NULL),
	mRaycastHitBuffer(NULL),
	mPreFilterShader(NULL),
	mPostFilterShader(NULL)
{
}

VehicleSceneQueryData::~VehicleSceneQueryData()
{
}

VehicleSceneQueryData* VehicleSceneQueryData::allocate(const physx::PxU32 maxNumVehicles, const physx::PxU32 maxNumWheelsPerVehicle, const physx::PxU32 maxNumHitPointsPerWheel, const physx::PxU32 numVehiclesInBatch, physx::PxBatchQueryPreFilterShader preFilterShader, physx::PxBatchQueryPostFilterShader postFilterShader, physx::PxAllocatorCallback& allocator)
{
	const physx::PxU32 sqDataSize = ((sizeof(VehicleSceneQueryData) + 15) & ~15);

	const physx::PxU32 maxNumWheels = maxNumVehicles * maxNumWheelsPerVehicle;
	const physx::PxU32 raycastResultSize = ((sizeof(physx::PxRaycastQueryResult) * maxNumWheels + 15) & ~15);
	const physx::PxU32 sweepResultSize = ((sizeof(physx::PxSweepQueryResult) * maxNumWheels + 15) & ~15);

	const physx::PxU32 maxNumHitPoints = maxNumWheels * maxNumHitPointsPerWheel;
	const physx::PxU32 raycastHitSize = ((sizeof(physx::PxRaycastHit) * maxNumHitPoints + 15) & ~15);
	const physx::PxU32 sweepHitSize = ((sizeof(physx::PxSweepHit) * maxNumHitPoints + 15) & ~15);

	const physx::PxU32 size = sqDataSize + raycastResultSize + raycastHitSize + sweepResultSize + sweepHitSize;
	physx::PxU8* buffer = static_cast<physx::PxU8*>(allocator.allocate(size, NULL, NULL, 0));

	VehicleSceneQueryData* sqData = new(buffer) VehicleSceneQueryData();
	sqData->mNumQueriesPerBatch = numVehiclesInBatch * maxNumWheelsPerVehicle;
	sqData->mNumHitResultsPerQuery = maxNumHitPointsPerWheel;
	buffer += sqDataSize;

	sqData->mRaycastResults = reinterpret_cast<physx::PxRaycastQueryResult*>(buffer);
	buffer += raycastResultSize;

	sqData->mRaycastHitBuffer = reinterpret_cast<physx::PxRaycastHit*>(buffer);
	buffer += raycastHitSize;

	sqData->mSweepResults = reinterpret_cast<physx::PxSweepQueryResult*>(buffer);
	buffer += sweepResultSize;

	sqData->mSweepHitBuffer = reinterpret_cast<physx::PxSweepHit*>(buffer);
	buffer += sweepHitSize;

	for (physx::PxU32 i = 0; i < maxNumWheels; i++)
	{
		new(sqData->mRaycastResults + i) physx::PxRaycastQueryResult();
		new(sqData->mSweepResults + i) physx::PxSweepQueryResult();
	}

	for (physx::PxU32 i = 0; i < maxNumHitPoints; i++)
	{
		new(sqData->mRaycastHitBuffer + i) physx::PxRaycastHit();
		new(sqData->mSweepHitBuffer + i) physx::PxSweepHit();
	}

	sqData->mPreFilterShader = preFilterShader;
	sqData->mPostFilterShader = postFilterShader;

	return sqData;
}

void VehicleSceneQueryData::free(physx::PxAllocatorCallback& allocator)
{
	allocator.deallocate(this);
}

physx::PxBatchQuery* VehicleSceneQueryData::setUpBatchedSceneQuery(const physx::PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, physx::PxScene* scene)
{
	const physx::PxU32 maxNumQueriesInBatch = vehicleSceneQueryData.mNumQueriesPerBatch;
	const physx::PxU32 maxNumHitResultsInBatch = vehicleSceneQueryData.mNumQueriesPerBatch * vehicleSceneQueryData.mNumHitResultsPerQuery;

	physx::PxBatchQueryDesc sqDesc(maxNumQueriesInBatch, maxNumQueriesInBatch, 0);

	sqDesc.queryMemory.userRaycastResultBuffer = vehicleSceneQueryData.mRaycastResults + batchId * maxNumQueriesInBatch;
	sqDesc.queryMemory.userRaycastTouchBuffer = vehicleSceneQueryData.mRaycastHitBuffer + batchId * maxNumHitResultsInBatch;
	sqDesc.queryMemory.raycastTouchBufferSize = maxNumHitResultsInBatch;

	sqDesc.queryMemory.userSweepResultBuffer = vehicleSceneQueryData.mSweepResults + batchId * maxNumQueriesInBatch;
	sqDesc.queryMemory.userSweepTouchBuffer = vehicleSceneQueryData.mSweepHitBuffer + batchId * maxNumHitResultsInBatch;
	sqDesc.queryMemory.sweepTouchBufferSize = maxNumHitResultsInBatch;

	sqDesc.preFilterShader = vehicleSceneQueryData.mPreFilterShader;

	sqDesc.postFilterShader = vehicleSceneQueryData.mPostFilterShader;

	return scene->createBatchQuery(sqDesc);
}

physx::PxRaycastQueryResult* VehicleSceneQueryData::getRaycastQueryResultBuffer(const physx::PxU32 batchId)
{
	return (mRaycastResults + batchId * mNumQueriesPerBatch);
}

physx::PxSweepQueryResult* VehicleSceneQueryData::getSweepQueryResultBuffer(const physx::PxU32 batchId)
{
	return (mSweepResults + batchId * mNumQueriesPerBatch);
}

physx::PxU32 VehicleSceneQueryData::getQueryResultBufferSize() const
{
	return mNumQueriesPerBatch;
}
