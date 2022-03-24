#include "ppch.h"
#include "TempSceneThree.h"

#include "Project/EntitySystem/Components/TransformComponent.h"
#include "Project/EntitySystem/Components/LightRendererComponent.h"


namespace Project
{
	TempSceneThree::TempSceneThree(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
	{
		m_Renderer = renderer;
		m_SceneIndex = sceneIndex;

		m_AmbientColour = ambientColour;
		m_SpecularPower = specularPower;
		m_backgroundColour = backgroundColour;
		m_VsyncOn = vsyncOn;

		m_sceneManager = sceneManager;

		m_EnablePhysics = false;
	}

	TempSceneThree::TempSceneThree(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
	{
		m_Renderer = renderer;
		m_SceneIndex = sceneIndex;
		m_EnablePhysics = enablePhysics;


		m_AmbientColour = ambientColour;
		m_SpecularPower = specularPower;
		m_backgroundColour = backgroundColour;
		m_VsyncOn = vsyncOn;

		m_sceneManager = sceneManager;
		
	}

	bool TempSceneThree::InitGeometry()
	{
		
		m_EntityManager = new EntityManager(m_Renderer);
		
		m_SceneCamera = new Camera();
		
		std::string path = "media/";
		m_EntityManager->CreateModelEntity("Floor", path + "Ground.x");
		m_EntityManager->CreateModelEntity("Test Cube", path + "Cube.x", path + "brick1.jpg");
		
		if (m_EnablePhysics)
		{
			m_PhysicsSystem = NewPhysics(m_sceneManager->GetWindowsProperties().PhysicsType);

			if (!m_PhysicsSystem->InitPhysics())
				m_Log.ErrorMessage(m_Renderer->GetWindowsProperties(), "Failed to Initialise Physics");

			// Serialise may not be needed
			//physx::PxSerializationRegistry* mSerializationRegistry = physx::PxSerialization::createSerializationRegistry(*m_PhysicsSystem->GetPhysics());
			physx::PxInitVehicleSDK(*m_PhysicsSystem->GetPhysics()/*, mSerializationRegistry*/);
			physx::PxVehicleSetBasisVectors(physx::PxVec3(0, 1, 0), physx::PxVec3(0, 0, 1));
			physx::PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);



			m_Material = m_PhysicsSystem->GetPhysics()->createMaterial(0, 0, 0);

			// Set Actors and shapes here
			m_BoxActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform({ 00.0f, 40.0f, 0.0f }));
			m_BoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor, physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *m_Material);

			m_FloorActor = m_PhysicsSystem->GetPhysics()->createRigidStatic({ 0.0f, 0.0f, 0.0f });
			m_FloorBoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_FloorActor, physx::PxBoxGeometry(1000.0f, 0.01f, 1000.0f), *m_Material);

			m_PhysicsSystem->GetScene()->addActor(*m_BoxActor);
			m_PhysicsSystem->GetScene()->addActor(*m_FloorActor);
			
		}

		return true;
	}

	bool TempSceneThree::InitScene()
	{
		
		if (m_EnablePhysics && m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"))
		{
			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"));

			CVector3 vect;
			vect.x = m_BoxActor->getGlobalPose().p.x;
			vect.y = m_BoxActor->getGlobalPose().p.y;
			vect.z = m_BoxActor->getGlobalPose().p.z;
			comp->SetPosition(vect);
		}


		m_SceneCamera->SetPosition({ 0, 10, -50 });
		m_SceneCamera->SetRotation({ 0, 0, 0 });
		return true;
	}

	void TempSceneThree::RenderScene()
	{
		m_EntityManager->RenderAllEntities();
	}

	void TempSceneThree::UpdateScene(float frameTime)
	{
		if (m_EnablePhysics)
		{
			m_PhysicsSystem->GetScene()->simulate(frameTime);

			if (m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"))
			{

				TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"));

				CVector3 vect;
				vect.x = m_BoxActor->getGlobalPose().p.x;
				vect.y = m_BoxActor->getGlobalPose().p.y;
				vect.z = m_BoxActor->getGlobalPose().p.z;
				comp->SetPosition(vect);
			}
			
			m_PhysicsSystem->GetScene()->fetchResults(true);
		}

		m_EntityManager->UpdateAllEntities(frameTime);

		m_SceneCamera->Control(frameTime);
		if (KeyHit(Key_L))
		{
			
			m_sceneManager->RemoveSceneAtIndex(0);
			
			m_sceneManager->LoadScene(1);
			
		}
		
	}

	void TempSceneThree::ReleaseResources()
	{
		
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)    m_EntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);

		if (m_EnablePhysics && m_WheelsSimData != nullptr) m_WheelsSimData->free();
		if (m_EnablePhysics && m_PhysicsSystem != nullptr) physx::PxCloseVehicleSDK();
		
		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();

		
		
		
	}

	void TempSceneThree::VehicleCreation()
	{
		//m_WheelsSimData = physx::PxVehicleWheelsSimData::allocate(numWheels);
		////SetupWheelsSimulationData();

		//// SetupDriveSimData function here

		//m_VehicleActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic({ 10.0f, 10.0f, 10.0f });
		//// SetupVehicle actor function here
		//m_PhysicsSystem->GetScene()->addActor(*m_VehicleActor);

		//m_VehicleDrive4W = physx::PxVehicleDrive4W::allocate(numWheels);
		//m_VehicleDrive4W->setup(m_PhysicsSystem->GetPhysics(), m_VehicleActor, *m_WheelsSimData, m_DriveSimData, numWheels - 4);

	}

	void TempSceneThree::SetupWheelsSimulationData(const physx::PxF32 wheelMass, const physx::PxF32 wheelMOI, 
		const physx::PxF32 wheelRadius, const physx::PxF32 wheelWidth, const physx::PxU32 numberWheels, 
		const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset, const physx::PxF32 chassisMass, 
		physx::PxVehicleWheelsSimData* wheelsSimData)
	{
		// Setup up the wheels
		physx::PxVehicleWheelData wheels[PX_MAX_NB_WHEELS]; //PX_MAX_NB_WHEELS max wheels is 20
		{
			// Setup wheel data structures with mass, moi, radius, width.
			for (physx::PxU32 i = 0; i < numberWheels; ++i)
			{
				wheels[i].mMass = wheelMass;
				wheels[i].mMOI = wheelMOI;
				wheels[i].mRadius = wheelRadius;
				wheels[i].mWidth = wheelWidth;
			}

			// Enable the handbrake for the rear wheels only
			wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxBrakeTorque = 4000.0f;
			wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxBrakeTorque = 4000.0f;
			// Enable seteering for the front wheels only
			wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = physx::PxPi * 0.3333f;
			wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = physx::PxPi * 0.3333f;
		}

		// Setup the tires
		physx::PxVehicleTireData tires[PX_MAX_NB_WHEELS];
		{
			for (physx::PxU32 i = 0; i < numberWheels; ++i)
			{
				tires[i].mType = TIRE_TYPE_NORMAL;
			}
		}

		// Setup the suspensions
		physx::PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
		{
			// Compute the mass supported by each suspension spring
			physx::PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
			physx::PxVehicleComputeSprungMasses(numberWheels, wheelCenterActorOffsets, chassisCMOffset, chassisMass, 1, suspSprungMasses);

			// Set the suspension data
			for (physx::PxU32 i = 0; i < numberWheels; ++i)
			{
				suspensions[i].mMaxCompression = 0.3f;
				suspensions[i].mMaxDroop = 0.1f;
				suspensions[i].mSpringStrength = 35000.0f;
				suspensions[i].mSpringDamperRate = 4500.0f;
				suspensions[i].mSprungMass = suspSprungMasses[i];
			}

			// Set the camber angles
			const physx::PxF32 camberAngleAtRest = 0.0f;
			const physx::PxF32 camberAngleAtMaxDroop = 0.01f;
			const physx::PxF32 camberAngleAtMaxCompression = 0.1f;

			for (physx::PxU32 i = 0; i < numberWheels; i += 2)
			{
				suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
				suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
				suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtRest;
				suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtRest;
				suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtRest;
				suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtRest;
			}
		}

		// Setup the wheel geometry
		physx::PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
		physx::PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
		physx::PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
		physx::PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
		{
			for (physx::PxU32 i = 0; i < numberWheels; i++)
			{
				// Vertical suspension Travel
				suspTravelDirections[i] = physx::PxVec3(0, -1, 0);

				// Wheel centre offset is Offset from rigid body centre of mass
				wheelCentreCMOffsets[i] = wheelCenterActorOffsets[i] - chassisCMOffset;

				// Suspension force application point 0.3 metres below rigid body centre of mass
				suspForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

				// Tire Force application point 0.3 metres below rigid body centre of mass
				tireForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
			}
		}

		physx::PxFilterData qryFilterData;
		SetupNonDrivableSurface(qryFilterData);

		// Set the wheel, tire and suspension data
		// Set the geomtry data
		// Set the query filter data
		for (physx::PxU32 i = 0; i < numberWheels; ++i)
		{
			wheelsSimData->setWheelData(i, wheels[i]);
			wheelsSimData->setTireData(i, tires[i]);
			wheelsSimData->setSuspensionData(i, suspensions[i]);
			wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
			wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
			wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
			wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
			wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
			wheelsSimData->setWheelShapeMapping(i, physx::PxI32(i));
		}

		//Add a front and rear anti-roll bar
		physx::PxVehicleAntiRollBarData barFront;
		barFront.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
		barFront.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
		barFront.mStiffness = 10000.0f;
		wheelsSimData->addAntiRollBarData(barFront);
		physx::PxVehicleAntiRollBarData barRear;
		barRear.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT;
		barRear.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
		barRear.mStiffness = 10000.0f;
		wheelsSimData->addAntiRollBarData(barRear);
	}

	physx::PxRigidDynamic* TempSceneThree::CreateVehicleActor(const physx::PxVehicleChassisData& chassisData, 
		physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numberWheels,
		const physx::PxFilterData& wheelSimFilterData, physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, 
		const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData, physx::PxPhysics& physics)
	{
		// We need a rigid body actor for the vehicle
		// Don't forget to add the actor to the scene after setting up the associated vehicle
		physx::PxRigidDynamic* actor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));

		// Wheels and chassis query filter data
		// Optional: cars don't drive on other cars
		physx::PxFilterData wheelQryFilterData;
		// TODO setupNonDrivableSurface function
		physx::PxFilterData chassisQryFilterData;
		// setupNonDrivableSurface function

		// Add all the wheels shape to the actor.
		for (physx::PxU32 i = 0; i < numberWheels; ++i)
		{
			physx::PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
			physx::PxShape* wheelShape = physx::PxRigidActorExt::createExclusiveShape(*actor, geom, *wheelMaterials[i]);
			wheelShape->setQueryFilterData(wheelQryFilterData);
			wheelShape->setSimulationFilterData(wheelSimFilterData);
			wheelShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		}

		// Add the chassis shapes to the actor
		for (physx::PxU32 i = 0; i < numChassisMeshes; ++i)
		{
			physx::PxShape* chassisShape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxConvexMeshGeometry(chassisConvexMeshes[i]), 
				*chassisMaterials[i]);
			chassisShape->setQueryFilterData(chassisQryFilterData);
			chassisShape->setSimulationFilterData(chassisSimFilterData);
			chassisShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		}

		actor->setMass(chassisData.mMass);
		actor->setMassSpaceInertiaTensor(chassisData.mMOI);
		actor->setCMassLocalPose(physx::PxTransform(chassisData.mCMOffset, physx::PxQuat(physx::PxIdentity)));

		return actor;
	}

	physx::PxVehicleDrive4W* TempSceneThree::CreateVehicle4W(const VehicleDesc& vehicle4WDesc, physx::PxPhysics* physics, physx::PxCooking* cooking)
	{
		const physx::PxVec3 chassisDims = vehicle4WDesc.chassisDims;
		const physx::PxF32 wheelWidth = vehicle4WDesc.wheelWidth;
		const physx::PxF32 wheelRadius = vehicle4WDesc.wheelRadius;
		const physx::PxU32 numberWheels = vehicle4WDesc.numWheels;

		const physx::PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
		const physx::PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

		// Construct a physx actor with shapes for the chassis and wheels
		// Set the rigid body mass, moment of inertia and centre of mass offset
		physx::PxRigidDynamic* veh4Actor = NULL;
		{
			// Construct a convex mesh for a cylindrical wheel
			physx::PxConvexMesh* wheelMesh = CreateWheelMesh(wheelWidth, wheelRadius, *physics, *cooking);
			// Assume all wheels are identical for simplicity
			physx::PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
			physx::PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

			// Set the meshes and materials for driven wheels
			for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT; ++i)
			{
				wheelConvexMeshes[i] = wheelMesh;
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}

			// Set the meshes and materials for the non driven wheels
			for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numberWheels; ++i)
			{
				wheelConvexMeshes[i] = wheelMesh;
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}

			// Chassis just has a single convex shape for simplicity 
			physx::PxConvexMesh* chassisConvexMesh = CreateChassisMesh(chassisDims, *physics, *cooking);
			physx::PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
			physx::PxMaterial* chassisMaterials[1] = { vehicle4WDesc.chassisMaterial };

			// Rigid body data
			physx::PxVehicleChassisData rigidBodyData;
			rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
			rigidBodyData.mMass = vehicle4WDesc.chassisMass;
			rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

			veh4Actor = CreateVehicleActor(rigidBodyData,
				wheelMaterials, wheelConvexMeshes, numberWheels, wheelSimFilterData,
				chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData,
				*physics);
		}

		// Setup the sim data for the wheels
		physx::PxVehicleWheelsSimData* wheelsSimData = physx::PxVehicleWheelsSimData::allocate(numberWheels);
		{
			// Compute the wheel centre offsets from the origin
			physx::PxVec3 wheelCentreActorOffsets[PX_MAX_NB_WHEELS];
			const physx::PxF32 frontZ = chassisDims.z * 3.0f;
			const physx::PxF32 rearZ = -chassisDims.z * 3.0f;
			ComputeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numberWheels, wheelCentreActorOffsets);

			SetupWheelsSimulationData(vehicle4WDesc.wheelMass, vehicle4WDesc.wheelMOI, wheelRadius, wheelWidth,
				numberWheels, wheelCentreActorOffsets,
				vehicle4WDesc.chassisCMOffset, vehicle4WDesc.chassisMass,
				wheelsSimData);
		}

		// Setup sim data for the vehicle drive model
		physx::PxVehicleDriveSimData4W driveSimData;
		{
			// Diff
			physx::PxVehicleDifferential4WData diff;
			diff.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
			driveSimData.setDiffData(diff);

			// Engine
			physx::PxVehicleEngineData engine;
			engine.mPeakTorque = 500.0f;
			engine.mMaxOmega = 600.0f; //Approx 6000rpm
			driveSimData.setEngineData(engine);

			// Gears
			physx::PxVehicleGearsData gears;
			gears.mSwitchTime = 0.5f;
			driveSimData.setGearsData(gears);

			// Clutch
			physx::PxVehicleClutchData clutch;
			clutch.mStrength = 10.0f;
			driveSimData.setClutchData(clutch);

			// Ackerman steer accuracy
			physx::PxVehicleAckermannGeometryData ackermann;
			ackermann.mAccuracy = 1.0f;
			ackermann.mAxleSeparation =
				wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
				wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
			ackermann.mFrontWidth =
				wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
			ackermann.mRearWidth =
				wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
			driveSimData.setAckermannGeometryData(ackermann);
		}

		// Create a vehicle from the wheels and drive sim data;
		physx::PxVehicleDrive4W* vehDrive4W = physx::PxVehicleDrive4W::allocate(numberWheels);
		vehDrive4W->setup(physics, veh4Actor, *wheelsSimData, driveSimData, numberWheels - 4);

		// Configure the userData
		ConfigureUserData(vehDrive4W, vehicle4WDesc.actorUserData, vehicle4WDesc.shapeUserDatas);

		// Free the sim data because its not needed anymore
		wheelsSimData->free();

		return vehDrive4W;
	}

	physx::PxConvexMesh* TempSceneThree::CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking)
	{
		// Create descriptor for convex mesh
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = numVerts;
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = verts;
		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxConvexMesh* convexMesh = NULL;
		physx::PxDefaultMemoryOutputStream buf;
		if (cooking.cookConvexMesh(convexDesc, buf))
		{
			physx::PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
			convexMesh = physics.createConvexMesh(id);
		}

		return convexMesh;
	}

	physx::PxConvexMesh* TempSceneThree::CreateWheelMesh(const physx::PxF32 width, const physx::PxF32 radius, physx::PxPhysics& physics, physx::PxCooking& cooking)
	{
		physx::PxVec3 points[2 * 16];
		for (physx::PxU32 i = 0; i < 16; i++)
		{
			const physx::PxF32 cosTheta = physx::PxCos(i * physx::PxPi * 2.0f / 16.0f);
			const physx::PxF32 sinTheta = physx::PxSin(i * physx::PxPi * 2.0f / 16.0f);
			const physx::PxF32 y = radius * cosTheta;
			const physx::PxF32 z = radius * sinTheta;
			points[2 * i + 0] = physx::PxVec3(-width / 2.0f, y, z);
			points[2 * i + 1] = physx::PxVec3(+width / 2.0f, y, z);
		}

		return CreateConvexMesh(points, 32, physics, cooking);
	}

	physx::PxConvexMesh* TempSceneThree::CreateChassisMesh(const physx::PxVec3 dims, physx::PxPhysics& physics, physx::PxCooking& cooking)
	{
		const physx::PxF32 x = dims.x * 0.5f;
		const physx::PxF32 y = dims.y * 0.5f;
		const physx::PxF32 z = dims.z * 0.5f;
		physx::PxVec3 verts[8] =
		{
			physx::PxVec3(x,y,-z),
			physx::PxVec3(x,y,z),
			physx::PxVec3(x,-y,z),
			physx::PxVec3(x,-y,-z),
			physx::PxVec3(-x,y,-z),
			physx::PxVec3(-x,y,z),
			physx::PxVec3(-x,-y,z),
			physx::PxVec3(-x,-y,-z)
		};

		return CreateConvexMesh(verts, 8, physics, cooking);
	}

	void TempSceneThree::ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32 wheelWidth, const physx::PxF32 wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets)
	{
		//chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
		//The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
		//Compute a position for the front wheel and the rear wheel along the z-axis.
		//Compute the separation between each wheel along the z-axis.
		const physx::PxF32 numLeftWheels = numWheels / 2.0f;
		const physx::PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f);
		//Set the outside of the left and right wheels to be flush with the chassis.
		//Set the top of the wheel to be just touching the underside of the chassis.
		//Begin by setting the rear-left/rear-right/front-left,front-right wheels.
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] =	  physx::PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] =  physx::PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] =  physx::PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1) * deltaZ);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1) * deltaZ);
		//Set the remaining wheels.
		for (physx::PxU32 i = 2, wheelCount = 4; i < numWheels - 2; i += 2, wheelCount += 2)
		{
			wheelCentreOffsets[wheelCount + 0] = physx::PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
			wheelCentreOffsets[wheelCount + 1] = physx::PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
		}
	}

	void TempSceneThree::ConfigureUserData(physx::PxVehicleWheels* vehicle, ActorUserData* actorUserData, ShapeUserData* shapeUserDatas)
	{
		if (actorUserData)
		{
			vehicle->getRigidDynamicActor()->userData = actorUserData;
			actorUserData->vehicle = vehicle;
		}

		if (shapeUserDatas)
		{
			physx::PxShape* shapes[PX_MAX_NB_WHEELS + 1];
			vehicle->getRigidDynamicActor()->getShapes(shapes, PX_MAX_NB_WHEELS + 1);
			for (physx::PxU32 i = 0; i < vehicle->mWheelsSimData.getNbWheels(); i++)
			{
				const physx::PxI32 shapeId = vehicle->mWheelsSimData.getWheelShapeMapping(i);
				shapes[shapeId]->userData = &shapeUserDatas[i];
				shapeUserDatas[i].isWheel = true;
				shapeUserDatas[i].wheelId = i;
			}
		}
	}

	
	
}