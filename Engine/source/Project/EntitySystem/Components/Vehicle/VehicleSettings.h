#pragma once
#include "Project/Interfaces/IPhysics.h"
#include "Project/EntitySystem/Components/Vehicle/ComponentVehicleHelpers.h"

namespace Project
{
	enum class HandBrake
	{
		RearWheelsOnly = 0,
		FrontWheelsOnly,
		AllWheels
	};

	
	class VehicleSettings
	{
	public:
		VehicleSettings() : m_IsAuto(true), m_ChassisMass(1500.0f), m_Diff(DefaultDifferetial()),
			m_Engine(DefaultEngine()), m_Gears(DefaultGears()), m_Clutch(DefaultClutch()),
			m_WheelMass(20.0f), m_MaxSteer(physx::PxPi * 0.3333f), m_NumWheels(4), m_HandBrake(HandBrake::RearWheelsOnly),
			m_MaxHandBrakeTorque(4000.0f), m_FrontRollBarStiffness(10000.0f), m_RearRollBarStiffness(10000.0f),
			m_TireType(TIRE_TYPE_NORMAL)
		{
			for (int i = 0; i < m_NumWheels; ++i)
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
		physx::PxF32 GetFrontAntiRollBarStiffness() { return m_FrontRollBarStiffness; }
		physx::PxF32 GetRearAntiRollBarStiffness() { return m_RearRollBarStiffness; }
		int GetTireType() { return m_TireType; }

		const physx::PxVehicleDifferential4WData DefaultDifferetial();
		const physx::PxVehicleEngineData DefaultEngine();
		const physx::PxVehicleGearsData DefaultGears();
		const physx::PxVehicleClutchData DefaultClutch();
		const physx::PxVehicleSuspensionData DefaultSuspension();


		void SetChassisMass(physx::PxF32 mass) { m_ChassisMass = mass; }
		void SetWheelMass(physx::PxF32 mass) { m_WheelMass = mass; }
		void SetMaxSteer(physx::PxF32 steer) { m_MaxSteer = steer; }

		void SetDifferential(physx::PxVehicleDifferential4WData diff) { m_Diff = diff; }
		void SetEngine(physx::PxVehicleEngineData engine) { m_Engine = engine; }
		void SetGears(physx::PxVehicleGearsData gears) { m_Gears = gears; }
		void SetClutch(physx::PxVehicleClutchData clutch) { m_Clutch = clutch; }
		void SetSuspension(physx::PxVehicleSuspensionData susp[], int size) 
		{ 
			for(int i = 0; i < size; ++i)
			{
				m_Suspension[i] = susp[i];
			}
		}

		void SetFrontAntiRollBarStiffness(physx::PxF32 stiffness) { m_FrontRollBarStiffness = stiffness; }
		void SetRearAntiRollBarStiffness(physx::PxF32 stiffness) { m_RearRollBarStiffness = stiffness; }

		void SetTires(int tire) { m_TireType = tire; }
		
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

		physx::PxF32 m_FrontRollBarStiffness;
		physx::PxF32 m_RearRollBarStiffness;
		
		int m_TireType;
	};
}
