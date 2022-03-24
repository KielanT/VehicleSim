#pragma once
#include "Project/SceneSystem/IScene.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/Interfaces/IPhysics.h"
#include "Project/EntitySystem/EntityManager.h"
#include "Project/SceneSystem/CDirectX11SceneManager.h"

namespace Project
{
	class TempSceneThree : public IScene
	{
	public:
		TempSceneThree(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		TempSceneThree(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		virtual bool InitGeometry() override;

		virtual bool InitScene() override;

		virtual void RenderScene() override;

		virtual void UpdateScene(float frameTime) override;

		virtual void ReleaseResources() override;

		virtual int GetSceneIndex() override { return m_SceneIndex; }

		virtual Camera* GetCamera() override { return m_SceneCamera; }

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

		IRenderer* m_Renderer;
		int m_SceneIndex;
		bool m_EnablePhysics;

		CVector3 m_AmbientColour;
		float m_SpecularPower;
		ColourRGBA m_backgroundColour;
		bool m_VsyncOn;

		Camera* m_SceneCamera = nullptr;

		EntityManager* m_EntityManager = nullptr;
		EntityManager* m_LightEntityManager = nullptr;

		CDirectX11SceneManager* m_sceneManager;

		IPhysics* m_PhysicsSystem;

		physx::PxMaterial* m_Material = nullptr;

		// Temp TODO create components
		physx::PxShape* m_FloorBoxShape = nullptr;
		physx::PxShape* m_BoxShape = nullptr;

		physx::PxRigidDynamic* m_BoxActor = nullptr;
		physx::PxRigidActor* m_FloorActor = nullptr;

	private:
		// Physx temp function
		void VehicleCreation();
		void SetupWheelsSimulationData(const physx::PxF32 wheelMass, const physx::PxF32 wheelMOI, const physx::PxF32 wheelRadius,
			const physx::PxF32 wheelWidth, const physx::PxU32 numberWheels, const physx::PxVec3* wheelCenterActorOffsets,
			const physx::PxVec3& chassisCMOffset, const physx::PxF32 chassisMass, physx::PxVehicleWheelsSimData* wheelsSimData);

		physx::PxRigidDynamic* CreateVehicleActor
		(const physx::PxVehicleChassisData& chassisData,
			physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numberWheels, const physx::PxFilterData& wheelSimFilterData,
			physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData,
			physx::PxPhysics& physics);

		

		void SetupNonDrivableSurface(physx::PxFilterData& filterData)
		{
			filterData.word3 = UNDRIVABLE_SURFACE;
		}

	private:
		// Physx temp vars
		//const physx::PxU32 numWheels = 4;

		physx::PxVehicleWheelsSimData* m_WheelsSimData = nullptr;
		physx::PxVehicleDriveSimData4W m_DriveSimData;
		physx::PxRigidDynamic* m_VehicleActor = nullptr;
		physx::PxVehicleDrive4W* m_VehicleDrive4W = nullptr;

		// Taking form SampleVehicle_VehicleManager.h and SnippetVehicletTireFriction.h
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

		enum
		{
			DRIVABLE_SURFACE = 0xffff0000,
			UNDRIVABLE_SURFACE = 0x0000ffff
		};

		struct ActorUserData
		{
			ActorUserData()
				: vehicle(NULL),
				actor(NULL)
			{
			}

			const physx::PxVehicleWheels* vehicle;
			const physx::PxActor* actor;
		};

		struct ShapeUserData
		{
			ShapeUserData()
				: isWheel(false),
				wheelId(0xffffffff)
			{
			}

			bool isWheel;
			physx::PxU32 wheelId;
		};

		struct VehicleDesc
		{
			VehicleDesc()
				: chassisMass(0.0f),
				chassisDims(physx::PxVec3(0.0f, 0.0f, 0.0f)),
				chassisMOI(physx::PxVec3(0.0f, 0.0f, 0.0f)),
				chassisCMOffset(physx::PxVec3(0.0f, 0.0f, 0.0f)),
				chassisMaterial(NULL),
				wheelMass(0.0f),
				wheelWidth(0.0f),
				wheelRadius(0.0f),
				wheelMOI(0.0f),
				wheelMaterial(NULL),
				actorUserData(NULL),
				shapeUserDatas(NULL)
			{
			}

			physx::PxF32 chassisMass;
			physx::PxVec3 chassisDims;
			physx::PxVec3 chassisMOI;
			physx::PxVec3 chassisCMOffset;
			physx::PxMaterial* chassisMaterial;
			physx::PxFilterData chassisSimFilterData;  //word0 = collide type, word1 = collide against types, word2 = PxPairFlags

			physx::PxF32 wheelMass;
			physx::PxF32 wheelWidth;
			physx::PxF32 wheelRadius;
			physx::PxF32 wheelMOI;
			physx::PxMaterial* wheelMaterial;
			physx::PxU32 numWheels;
			physx::PxFilterData wheelSimFilterData;	//word0 = collide type, word1 = collide against types, word2 = PxPairFlags

			ActorUserData* actorUserData;
			ShapeUserData* shapeUserDatas;
		};

		physx::PxVehicleDrive4W* CreateVehicle4W(const VehicleDesc& vehicle4WDesc, physx::PxPhysics* physics, physx::PxCooking* cooking);
		
		static physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking);
		physx::PxConvexMesh* CreateWheelMesh(const physx::PxF32 width, const physx::PxF32 radius, physx::PxPhysics& physics, physx::PxCooking& cooking);
		physx::PxConvexMesh* CreateChassisMesh(const physx::PxVec3 dims, physx::PxPhysics& physics, physx::PxCooking& cooking);
		void ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims,
			const physx::PxF32 wheelWidth, const physx::PxF32 wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets);
		void ConfigureUserData(physx::PxVehicleWheels* vehicle, ActorUserData* actorUserData, ShapeUserData* shapeUserDatas);
	};
}

