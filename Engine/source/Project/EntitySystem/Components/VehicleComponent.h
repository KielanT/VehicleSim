#pragma once
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/Interfaces/IPhysics.h"
#include "TransformComponent.h"

namespace Project
{
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

	enum
	{
		TIRE_TYPE_NORMAL = 0,
		TIRE_TYPE_WORN,
		MAX_NUM_TIRE_TYPES
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

	class VehicleComponent : public EntityComponent
	{
	public:
		VehicleComponent(Entity* entity, TEntityUID UID, IPhysics* physics, VehicleSettings vehSettings) : EntityComponent("VehicleComponent", UID, entity)
		{
			m_Entity = entity;
			if (m_Entity != nullptr && physics != nullptr)
			{
				m_Physics = physics;
				m_VehicleSettings = vehSettings;

				SetupVehicle();
			}
		}
		~VehicleComponent();
		
		virtual bool Update(float frameTime) override;
		
		enum
		{
			DRIVABLE_SURFACE = 0xffff0000,
			UNDRIVABLE_SURFACE = 0x0000ffff
		};

		void SetupDrivableSurface(physx::PxFilterData& filterData)
		{
			filterData.word3 = static_cast<physx::PxU32>(DRIVABLE_SURFACE);
		}

		void SetupNonDrivableSurface(physx::PxFilterData& filterData)
		{
			filterData.word3 = UNDRIVABLE_SURFACE;
		}

	private:
		void SetupVehicle();
		
		
	
		// TEMP from scene five
		struct VehicleDesc
		{
			VehicleDesc()
				: chassisMass(0.0f),
				chassisMaterial(NULL),
				wheelMass(0.0f),
				wheelMaterial(NULL)
			{
			}

			physx::PxF32 chassisMass;
			physx::PxMaterial* chassisMaterial;
			physx::PxFilterData chassisSimFilterData;  //word0 = collide type, word1 = collide against types, word2 = PxPairFlags

			physx::PxF32 wheelMass;
			physx::PxMaterial* wheelMaterial;
			physx::PxU32 numWheels;
			physx::PxFilterData wheelSimFilterData;	//word0 = collide type, word1 = collide against types, word2 = PxPairFlags
		};
		
		VehicleDesc InitVehicleDesc();
		physx::PxVehicleDrive4W* CreateVehicle4W(const VehicleDesc& vehicle4WDesc);

		physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts);
		physx::PxConvexMesh* CreateWheelMesh(int index);
		physx::PxConvexMesh* CreateChassisMesh(int index);

		void MakeWheelWidthsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii);
		
		physx::PxVec3 MakeChassis(physx::PxConvexMesh* chassisConvexMesh);

		physx::PxRigidDynamic* CreateVehicleActor(const physx::PxVehicleChassisData& chassisData,
			physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, const physx::PxFilterData& wheelSimFilterData,
			physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData);
		void ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32* wheelWidth,
			const physx::PxF32* wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets);
		void SetupWheelsSimulationData
		(const physx::PxF32 wheelMass, const physx::PxF32* wheelMOI, const physx::PxF32* wheelRadius, const physx::PxF32* wheelWidth,
			const physx::PxU32 numWheels, const physx::PxVec3* wheelCenterActorOffsets,
			const physx::PxVec3& chassisCMOffset, const physx::PxF32 chassisMass,
			physx::PxVehicleWheelsSimData* wheelsSimData);


	private:
		Entity* m_Entity;
		
		IPhysics* m_Physics;
		VehicleSettings m_VehicleSettings;

		physx::PxVehicleDrive4W* m_Vehicle;

		TransformComponent* m_Transform;

		physx::PxMaterial* m_Material;
		
		physx::PxVehicleDrivableSurfaceToTireFrictionPairs* m_FrictionPairs;
		
	};

	
}
