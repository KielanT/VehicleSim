#pragma once
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/Interfaces/IPhysics.h"
#include "TransformComponent.h"
#include "ComponentVehicleHelpers.h"

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
