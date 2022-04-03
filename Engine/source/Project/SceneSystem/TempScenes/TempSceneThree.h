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

		//physx::PxVec3 m_ChassisDims;

	private:
		// Physx temp function
		/*void SetupWheelsSimulationData(
			const physx::PxVec3& chassisCMOffset, const physx::PxF32 chassisMass,
			physx::PxVehicleWheelsSimData* wheelsSimData);

		physx::PxRigidDynamic* CreateVehicleActor
		(const physx::PxVehicleChassisData& chassisData,
			physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numberWheels, const physx::PxFilterData& wheelSimFilterData,
			physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData,
			physx::PxPhysics& physics);

		

		void SetupNonDrivableSurface(physx::PxFilterData& filterData)
		{
			filterData.word3 = UNDRIVABLE_SURFACE;
		}*/

	private:
		// Physx temp vars
		//const physx::PxU32 numWheels = 4;

		//physx::PxVehicleWheelsSimData* m_WheelsSimData = nullptr;
		//physx::PxVehicleDriveSimData4W m_DriveSimData;
		//physx::PxRigidDynamic* m_VehicleActor = nullptr;
		//physx::PxVehicleDrive4W* m_VehicleDrive4W = nullptr;

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

		//enum
		//{
		//	DRIVABLE_SURFACE = 0xffff0000,
		//	UNDRIVABLE_SURFACE = 0x0000ffff
		//};

		//struct ActorUserData
		//{
		//	ActorUserData()
		//		: vehicle(NULL),
		//		actor(NULL)
		//	{
		//	}

		//	const physx::PxVehicleWheels* vehicle;
		//	const physx::PxActor* actor;
		//};

		//struct ShapeUserData
		//{
		//	ShapeUserData()
		//		: isWheel(false),
		//		wheelId(0xffffffff)
		//	{
		//	}

		//	bool isWheel;
		//	physx::PxU32 wheelId;
		//};

		//struct VehicleDesc
		//{
		//	VehicleDesc()
		//		:

		//		chassisMass(1500.0f),
		//		actorUserData(NULL),
		//		shapeUserDatas(NULL)
		//	{
		//	}

		//	physx::PxF32 chassisMass;

		//	physx::PxFilterData chassisSimFilterData;  //word0 = collide type, word1 = collide against types, word2 = PxPairFlags


		//	
		//	physx::PxFilterData wheelSimFilterData;	//word0 = collide type, word1 = collide against types, word2 = PxPairFlags

		//	ActorUserData* actorUserData;
		//	ShapeUserData* shapeUserDatas;
		//};
		// 
		//Collision types and flags describing collision interactions of each collision type.
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
			COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		};

		//physx::PxVehicleDrive4W* CreateVehicle4W(const VehicleDesc& vehicle4WDesc, physx::PxPhysics* physics, physx::PxCooking* cooking);
		
		static physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking);
		physx::PxConvexMesh* CreateWheelMesh(int index, physx::PxPhysics& physics, physx::PxCooking& cooking);
		physx::PxConvexMesh* CreateChassisMesh(int index, physx::PxPhysics& physics, physx::PxCooking& cooking);

		//VehicleDesc InitVehicleDesc();

		physx::PxVec3 MakeChassis(physx::PxConvexMesh* mesh); //computeChassisAABBDimensions function from SampleVehicle_VehicleManager
		
		void MakeWheelWithsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii); // computeWheelWidthsAndRadii function from SampleVehicle_VehicleManager

		/*physx::PxVehicleChassisData m_ChassisData;
		physx::PxVehicleWheelData m_Wheels[4];
		physx::PxVec3 m_WheelCentreOffsets[4];*/

		/*physx::PxI32 vertexCountWheel;
		std::vector<physx::PxVec3> verticesWheel;*/

		physx::PxConvexMesh* m_ChassisConvexMesh;
		physx::PxConvexMesh* m_WheelConvexMeshes[4];
		physx::PxVec3 m_WheelCentreOffsets4[4];

		physx::PxVehicleDrive4W* m_Car;

		// Retry implementation
		physx::PxVehicleDrive4W* Create4WVehicle(); // Example from SampleVehicle_VehicleManager 
		void CreateVehicle4WSimulationData(physx::PxF32 chassisMass, physx::PxVehicleChassisData& chassisData, physx::PxVec3* wheelCentreOffsets, 
			physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32 wheelMass, physx::PxVehicleWheelsSimData& wheelsData, physx::PxVehicleDriveSimData4W& driveData);
		physx::PxRigidDynamic* CreateVehicleActor4W(physx::PxConvexMesh** wheelConvexMeshes, physx::PxConvexMesh* chassisConvexMesh, 
			physx::PxVehicleChassisData& chassisData);
		void SetupActor(physx::PxRigidDynamic* vehActor, physx::PxGeometry** wheelGeometries, physx::PxFilterData& vehQryFilterData, 
			physx::PxFilterData& wheelCollFilterData, physx::PxTransform* wheelLocalPoses, physx::PxGeometry** chassisGeometries, 
			physx::PxFilterData& chassisCollFilterData, physx::PxTransform* chassisLocalPoses, physx::PxVehicleChassisData& chassisData);

	};
}

