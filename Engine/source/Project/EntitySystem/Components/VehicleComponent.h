#pragma once
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/Interfaces/IPhysics.h"
#include "TransformComponent.h"
#include "ComponentVehicleHelpers.h"
#include "Physics/PhysX4.1/VehicleSceneQueryData.h"

namespace Project
{
	
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
				UpdateVehiclePosAndRot();
			}
		}
		~VehicleComponent();
		
		virtual bool Update(float frameTime) override;
		
	private:
		void SetupVehicle();

		physx::PxVehicleDrive4W* CreateVehicle();

		physx::PxConvexMesh* CreateWheelMesh(int index);
		physx::PxConvexMesh* CreateChassisMesh(int index);

		physx::PxRigidDynamic* CreateVehicleActor(const physx::PxVehicleChassisData& chassisData,
			physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, const physx::PxFilterData& wheelSimFilterData,
			physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData);
		void ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32* wheelWidth,
			const physx::PxF32* wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets);
		
		void MoveVehicle(float frameTime);
		void UpdateVehiclePosAndRot();
		void UpdateInput();

		
		

	private:
		Entity* m_Entity;
		
		IPhysics* m_Physics;
		VehicleSettings m_VehicleSettings;

		physx::PxVehicleDrive4W* m_Vehicle;

		TransformComponent* m_Transform;

		physx::PxMaterial* m_Material;
		
		physx::PxVehicleDrivableSurfaceToTireFrictionPairs* m_FrictionPairs;

		bool m_Accelerate = false;
		bool m_Left = false;
		bool m_Right = false;
		bool m_Brake = false;
		bool m_HandBrake = false;
		
		bool IsVehicleInAir = true;

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
