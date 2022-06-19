#pragma once
#include "Project/Interfaces/IPhysics.h"

namespace Project
{
	enum
	{
		TIRE_TYPE_NORMAL = 0,
		TIRE_TYPE_WORN,
		MAX_NUM_TIRE_TYPES
	};
		
	enum
	{
		SURFACE_TYPE_TARMAC = 0,
		MAX_NUM_SURFACE_TYPES
	};
	
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

	enum
	{
		DRIVABLE_SURFACE = 0xffff0000,
		UNDRIVABLE_SURFACE = 0x0000ffff
	};

	enum class HandBrake
	{
		RearWheelsOnly = 0,
		FrontWheelsOnly,
		AllWheels
	};

	enum class AntiRollBar
	{
		FrontOnly = 0,
		RearOnly,
		Both,
		None
	};

	class VehicleSettings
	{
	public:
		VehicleSettings() : m_IsAuto(true), m_ChassisMass(1500.0f), m_Diff(DefaultDifferetial()),
			m_Engine(DefaultEngine()), m_Gears(DefaultGears()), m_Clutch(DefaultClutch()),
			m_WheelMass(20.0f), m_MaxSteer(physx::PxPi * 0.3333f), m_NumWheels(4), m_HandBrake(HandBrake::RearWheelsOnly),
			m_MaxHandBrakeTorque(4000.0f), m_AntiRollBar(AntiRollBar::Both), m_RollBarStiffness(10000.0f)
		{
			for (int i = 0; m_NumWheels < 0; ++i)
			{
				m_Suspension[i] = DefaultSuspension();
			}
		}

		const int GetNumberOfWheels() { return m_NumWheels; }
		const physx::PxF32 GetChassisMass() { return m_ChassisMass; }
		const physx::PxF32 GetWheelMass() { return m_WheelMass; }

		const physx::PxVehicleDifferential4WData GetDiff() { return m_Diff; }
		const physx::PxVehicleEngineData GetEngine() { return m_Engine; }
		const physx::PxVehicleGearsData GetGears() { return m_Gears; }
		const physx::PxVehicleClutchData GetClutch() { return m_Clutch; }
		HandBrake GetHandBrake() { return m_HandBrake; }
		physx::PxReal GetHandBrakeTorque() { return m_MaxHandBrakeTorque; }
		physx::PxF32 GetMaxSteer() { return m_MaxSteer; }
		physx::PxVehicleSuspensionData GetSuspension(int index) { return m_Suspension[index]; }
		AntiRollBar GetAntiRollBar() { return m_AntiRollBar; }
		physx::PxF32 GetAntiRollBarStiffness() { return m_RollBarStiffness; }

		const physx::PxVehicleDifferential4WData DefaultDifferetial();
		const physx::PxVehicleEngineData DefaultEngine();
		const physx::PxVehicleGearsData DefaultGears();
		const physx::PxVehicleClutchData DefaultClutch();
		const physx::PxVehicleSuspensionData DefaultSuspension();

	private:

		bool m_IsAuto;

		// Main Vehicle Settings
		physx::PxF32 m_ChassisMass;

		physx::PxVehicleDifferential4WData m_Diff;
		physx::PxVehicleEngineData m_Engine;
		physx::PxVehicleGearsData m_Gears;
		physx::PxVehicleClutchData m_Clutch;

		// Wheel Settings
		physx::PxF32 m_WheelMass;

		physx::PxF32 m_MaxSteer;

		int m_NumWheels;

		physx::PxVehicleSuspensionData m_Suspension[PX_MAX_NB_WHEELS];

		HandBrake m_HandBrake;
		physx::PxReal m_MaxHandBrakeTorque;

		AntiRollBar m_AntiRollBar;

		physx::PxF32 m_RollBarStiffness;

	};

	void SetupWheelsSimulationData(VehicleSettings vehicleSettings, const physx::PxF32* wheelMOI, const physx::PxF32* wheelRadius, const physx::PxF32* wheelWidth,
		const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset, physx::PxVehicleWheelsSimData* wheelsSimData);

	static void DrivableSurface(physx::PxFilterData& filterData)
	{
		filterData.word3 = static_cast<physx::PxU32>(DRIVABLE_SURFACE);
	}

	static void NonDrivableSurface(physx::PxFilterData& filterData)
	{
		filterData.word3 = UNDRIVABLE_SURFACE;
	}
	
	physx::PxConvexMesh* CreateConvexMesh(IPhysics* physics, const physx::PxVec3* verts, const physx::PxU32 numVerts);	

	void MakeWheelWidthsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii);

	physx::PxVec3 MakeChassis(physx::PxConvexMesh* chassisConvexMesh);
}