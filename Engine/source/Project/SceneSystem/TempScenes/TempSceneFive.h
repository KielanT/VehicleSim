#pragma once
#include "Project/SceneSystem/IScene.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/Interfaces/IPhysics.h"
#include "Project/EntitySystem/EntityManager.h"
#include "Project/SceneSystem/CDirectX11SceneManager.h"
#include "Physics/PhysX4.1/VehicleSceneQueryData.h"

namespace Project
{
	class TempSceneFive : public IScene
	{
	public:
		TempSceneFive(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		TempSceneFive(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		virtual bool InitGeometry() override;

		virtual bool InitScene() override;

		virtual void RenderScene() override;

		virtual void UpdateScene(float frameTime) override;

		virtual void ReleaseResources() override;

		virtual int GetSceneIndex() override { return m_SceneIndex; }

		virtual std::shared_ptr<Camera> GetCamera() override { return m_SceneCamera; }

		// Scene Settings
		virtual void SetAmbientColour(CVector3 ambientColour) override { m_AmbientColour = ambientColour; }
		virtual void SetSpecularPower(float specularPower) override { m_SpecularPower = specularPower; }
		virtual void SetBackgroundColour(ColourRGBA backgroundColour) override { m_backgroundColour = backgroundColour; }
		virtual void SetVSync(bool VSync) override { m_VsyncOn = VSync; }

		virtual CVector3 GetAmbientColour() override { return m_AmbientColour; }
		virtual float GetSpecularPower() override { return m_SpecularPower; }
		virtual ColourRGBA GetBackgroundColour() override { return m_backgroundColour; }
		virtual bool GetVSync() override { return m_VsyncOn; }

	private:
		ErrorLogger m_Log;

		std::shared_ptr<IRenderer> m_Renderer;
		int m_SceneIndex;
		bool m_EnablePhysics;

		CVector3 m_AmbientColour;
		float m_SpecularPower;
		ColourRGBA m_backgroundColour;
		bool m_VsyncOn;

		std::shared_ptr<Camera> m_SceneCamera = nullptr;

		EntityManager* m_EntityManager = nullptr;
		EntityManager* m_LightEntityManager = nullptr;

		CDirectX11SceneManager* m_sceneManager;

		std::shared_ptr<IPhysics> m_PhysicsSystem = nullptr;

		physx::PxMaterial* m_Material = nullptr;

		physx::PxShape* m_FloorBoxShape = nullptr;
		physx::PxShape* m_BoxShape = nullptr;

		physx::PxRigidDynamic* m_BoxActor = nullptr;
		physx::PxRigidActor* m_FloorActor = nullptr;
		physx::PxRigidStatic* m_FloorPlane = nullptr;

		bool m_IsDevMode = false;
		
	private:
		void Gui();

		void CameraControl(float frameTime);

		// Vehicle Setup 
	private:
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

		physx::PxVehicleKeySmoothingData keySmoothingData =
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


		VehicleDesc InitVehicleDesc();

		physx::PxVehicleDrive4W* m_Vehicle4W;

		physx::PxVehicleDrive4W* CreateVehicle4W(const VehicleDesc& vehicle4WDesc);

		physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts);
		physx::PxConvexMesh* CreateWheelMesh(int index);
		physx::PxConvexMesh* CreateChassisMesh(int index);

		physx::PxRigidDynamic* CreateVehicleActor(const physx::PxVehicleChassisData& chassisData,
			physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, const physx::PxFilterData& wheelSimFilterData,
			physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData);

		void setupDrivableSurface(physx::PxFilterData& filterData)
		{
			filterData.word3 = static_cast<physx::PxU32>(DRIVABLE_SURFACE);
		}

		void setupNonDrivableSurface(physx::PxFilterData& filterData)
		{
			filterData.word3 = UNDRIVABLE_SURFACE;
		}


		void ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32* wheelWidth,
			const physx::PxF32* wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets);

		void SetupWheelsSimulationData
		(const physx::PxF32 wheelMass, const physx::PxF32* wheelMOI, const physx::PxF32* wheelRadius, const physx::PxF32* wheelWidth,
			const physx::PxU32 numWheels, const physx::PxVec3* wheelCenterActorOffsets,
			const physx::PxVec3& chassisCMOffset, const physx::PxF32 chassisMass,
			physx::PxVehicleWheelsSimData* wheelsSimData);

		physx::PxVec3 MakeChassis(physx::PxConvexMesh* chassisConvexMesh);

		void MakeWheelWidthsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii);

		// Get vehicle Moving

		VehicleSceneQueryData* m_VehicleSceneQueryData = NULL;
		physx::PxBatchQuery* m_BatchQuery = NULL;
		physx::PxDefaultAllocator m_Allocator;
		physx::PxVehicleDrivableSurfaceToTireFrictionPairs* frictionPairs;

		bool IsVehicleInAir = true; // Spawns in the air
		
		bool m_Accelerate = false;
		bool m_Left = false;
		bool m_Right = false;
		bool m_Brake = false;
		bool m_HandBrake = false;

		
		void MoveVehicle(float frameTime);
		void Controls();
		physx::PxVehicleDrivableSurfaceToTireFrictionPairs* CreateFrictionPairs(const physx::PxMaterial* defaultMaterial);
		physx::PxRigidStatic* CreateDrivablePlane(const physx::PxFilterData& simFilterData, physx::PxMaterial* material, physx::PxPhysics* physics);

		//void SetAutoBoxData(const physx::PxVehicleAutoBoxData& autobox);
	};
}
