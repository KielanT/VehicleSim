#pragma once
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/Interfaces/IPhysics.h"
#include "TransformComponent.h"
#include "Project/EntitySystem/Components/Vehicle/ComponentVehicleHelpers.h"
#include "Project/EntitySystem/Components/Vehicle/VehicleSettings.h"
#include "Physics/PhysX4.1/VehicleSceneQueryData.h"
#include "Physics/PhysX4.1/PhysxModelHelpers.h"
#include "Project/Lab/Camera.h"
#include "Data/PlayerSettings.h"


namespace Project
{
	
	class VehicleComponent : public EntityComponent
	{
	public:
		VehicleComponent(Entity* entity, TEntityUID UID, IPhysics* physics, VehicleSettings vehSettings) : EntityComponent("VehicleComponent", UID, entity)
		{
			// Sets up the vehicle with all the correct data
			m_Entity = entity;
			if (m_Entity != nullptr && physics != nullptr)
			{
				m_Physics = physics;
				m_VehicleSettings = vehSettings;

				PlayerSettings settings;
				PlayerControls controls;

				if (settings.LoadPlayerControls(controls))
					m_Controls = controls;
				else
					m_Controls = PlayerControls();


				SetupVehicle();
				UpdateVehiclePosAndRot();

				m_ResetPos = { 0.0f, 0.0f, 0.0f };

			}
		}
		~VehicleComponent();
		
		virtual bool Update(float frameTime) override;
		
		// Used for attach camera to the vehicle
		void AttachMainCamera(Camera* camera) { m_Camera = camera; };
		void DetachMainCamera() { m_Camera = nullptr; }

		physx::PxRigidDynamic* GetActor() { return m_Vehicle4W->getRigidDynamicActor(); }

		void Reset();
		void SetResetPos(CVector3 pos)
		{
			m_ResetPos = pos;
		}

		void GearsUI();

		void Release()
		{
			if (m_Physics != nullptr) physx::PxCloseVehicleSDK();
		}

		void EnableReset(bool reset) { m_IsResetEnabled = reset; }

	private:
		void SetupVehicle();

		physx::PxVehicleDrive4W* CreateVehicle4W();
		

		physx::PxRigidDynamic* CreateVehicleActor(const physx::PxVehicleChassisData& chassisData,
			physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, const physx::PxFilterData& wheelSimFilterData,
			physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData);

		
		void SetupWheelsSimulationData
		(const physx::PxF32 wheelMass, const physx::PxF32* wheelMOI, const physx::PxF32* wheelRadius, const physx::PxF32* wheelWidth,
			const physx::PxU32 numWheels, const physx::PxVec3* wheelCenterActorOffsets,
			const physx::PxVec3& chassisCMOffset, const physx::PxF32 chassisMass,
			physx::PxVehicleWheelsSimData* wheelsSimData);

		

		void MoveVehicle(float frameTime);
		void UpdateVehiclePosAndRot();
		void UpdateInput();

		void MoveCamera();
		
		
		CVector3 m_ResetPos;
		bool m_IsResetEnabled = true;

	private:
		Entity* m_Entity = nullptr;
		
		IPhysics* m_Physics = nullptr;
		VehicleSettings m_VehicleSettings;

		physx::PxVehicleDrive4W* m_Vehicle4W = nullptr;

		TransformComponent* m_Transform = nullptr;

		physx::PxMaterial* m_Material = nullptr;
		
		physx::PxVehicleDrivableSurfaceToTireFrictionPairs* m_FrictionPairs = nullptr;

		Camera* m_Camera = nullptr;

		bool m_Accelerate = false;
		bool m_Left = false;
		bool m_Right = false;
		bool m_Brake = false;
		bool m_HandBrake = false;
		bool m_GearUp = false;
		bool m_GearDown = false;
		
		bool IsVehicleInAir = false;

		PlayerControls m_Controls;

		// Data required for vehicle movement

		VehicleSceneQueryData* m_VehicleSceneQueryData = NULL;
		physx::PxBatchQuery* m_BatchQuery = NULL;
		physx::PxDefaultAllocator m_Allocator;

		physx::PxF32 SteerVsForwardSpeedDataComp[2 * 8] =
		{
			0.0f,		0.75f,
			5.0f,		0.75f,
			30.0f,		0.125f,
			120.0f,		0.1f,
			PX_MAX_F32, PX_MAX_F32,
			PX_MAX_F32, PX_MAX_F32,
			PX_MAX_F32, PX_MAX_F32,
			PX_MAX_F32, PX_MAX_F32
		};
		
		physx::PxVehicleKeySmoothingData KeySmoothingData =
		{
			{
				3.0f,	//rise rate eANALOG_INPUT_ACCEL		
				3.0f,	//rise rate eANALOG_INPUT_BRAKE		
				10.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
				2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT	
				2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT	
			},
			{
				5.0f,	//fall rate eANALOG_INPUT__ACCEL		
				5.0f,	//fall rate eANALOG_INPUT__BRAKE		
				10.0f,	//fall rate eANALOG_INPUT__HANDBRAKE	
				5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT	
				5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT	
			}
		};

		physx::PxVehicleDrive4WRawInputData m_VehicleInputData;
		
	};

	
}
