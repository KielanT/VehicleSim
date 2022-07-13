#pragma once
#include "Project/Interfaces/IPhysics.h"
#include "Physics/PhysX4.1/PhysxModelHelpers.h"
#include "Project/EntitySystem/Components/CollisionComponent.h"

// enums, variables and functions for creating vehicles

namespace Project
{
	// Create different tires that the vehicle can use
	enum
	{
		TIRE_TYPE_NORMAL = 0,
		TIRE_TYPE_WORN,
		TIRE_TYPE_WETS,
		TIRE_TYPE_SLICKS,
		TIRE_TYPE_ICE,
		TIRE_TYPE_MUD,
		MAX_NUM_TIRE_TYPES
	};
		
	// Different surface types (only tarmac is implemented)
	enum
	{
		SURFACE_TYPE_TARMAC = 0,
		SURFACE_TYPE_MUD,
		SURFACE_TYPE_SNOW,
		SURFACE_TYPE_GRASS,
		MAX_NUM_SURFACE_TYPES
	};
	
	// VALUES FROM PHYSX SAMPLE
	//Tire model friction for each combination of drivable surface type and tire type.
	static physx::PxF32 TireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
	{
		//NORMAL	WORN	WETS	SLICKS	ICE		MUD
		{1.00f,		0.1f,	1.10f,	1.15f,	1.10f,	1.10f}, // TARMAC
		{1.00f,		0.1f,	0.95f,	0.95f,	0.95f,	0.95f}, // MUD
		{1.00f,		0.1f,	0.70f,	0.70f,	0.70f,	0.70f}, // SNOW
		{1.00f,		0.1f,	0.80f,	0.80f,	0.80f,	0.80f} // GRASS
	};
	
	// The collision flags used for setting up collision
	enum
	{
		COLLISION_FLAG_GROUND = 1 << 0,
		COLLISION_FLAG_WHEEL = 1 << 1,
		COLLISION_FLAG_CHASSIS = 1 << 2,
		COLLISION_FLAG_OBSTACLE = 1 << 3,
		COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,
	
		COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
		COLLISION_FLAG_CHASSIS_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE
	};

	// Used to set if an object is drivable or not
	enum
	{
		DRIVABLE_SURFACE = 0xffff0000,
		UNDRIVABLE_SURFACE = 0x0000ffff
	};

	
	// Sets up the filter for drivable surfaces
	static void DrivableSurface(physx::PxFilterData& filterData)
	{
		filterData.word3 = static_cast<physx::PxU32>(DRIVABLE_SURFACE);
	}

	// Sets up the filter for nondrivable surfaces
	static void NonDrivableSurface(physx::PxFilterData& filterData)
	{
		filterData.word3 = UNDRIVABLE_SURFACE;
	}

	// Create wheel meshes 
	physx::PxConvexMesh* CreateWheelMesh(int index, Entity* entity, IPhysics* physics);
	
	// Create chassis mesh
	physx::PxConvexMesh* CreateChassisMesh(int index, Entity* entity, IPhysics* physics);
	
	// Compute the wheel widths and radii from the convex mesh
	void MakeWheelWidthsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii);

	// Creates the chassis from convex mesh
	physx::PxVec3 MakeChassis(physx::PxConvexMesh* chassisConvexMesh);
	
	// Compute all the wheel offsets
	void ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32* wheelWidth,
		const physx::PxF32* wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets);

	// Creates the friction pairs for friction surfaces
	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* CreateFrictionPairs(const physx::PxMaterial* defaultMaterial);

}