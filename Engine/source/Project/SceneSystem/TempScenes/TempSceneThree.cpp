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
		//m_EntityManager->CreateModelEntity("Floor", path + "Grid.x", path + "grid.jpg");
		//m_EntityManager->CreateModelEntity("Floor", path + "Grid.x", path + "grid2.jpg");

		m_EntityManager->CreateModelEntity("Test Cube", path + "Cube.x", path + "brick1.jpg");
		//m_EntityManager->CreateModelEntity("Car", path + "Hatchback/untitled.obj", path + "Hatchback/HatchbackYellow.png");
		m_EntityManager->CreateModelEntity("Car", path + "Compact/untitled1.obj", path + "Compact/CompactBlue.png");

		

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



			// Create vehicle
			//VehicleDesc vehicleDesc = InitVehicleDesc();
			//m_VehicleDrive4W = CreateVehicle4W(vehicleDesc, m_PhysicsSystem->GetPhysics(), m_PhysicsSystem->GetCooking());
			//physx::PxTransform startTransform(physx::PxVec3(0, /*(vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f)*/10.0f, 0), physx::PxQuat(physx::PxIdentity));

			//m_VehicleDrive4W->getRigidDynamicActor()->setGlobalPose({ 0, 0, 0 });

			m_Car = Create4WVehicle();
			m_Car->getRigidDynamicActor()->setGlobalPose({ 0, 0, 0 });

			// Set Actors and shapes here
			m_BoxActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform({ 0.0f, 40.0f, 50.0f }));
			m_BoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor, physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *m_Material);

			m_FloorActor = m_PhysicsSystem->GetPhysics()->createRigidStatic({ 0.0f, 0.0f, 0.0f });
			m_FloorBoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_FloorActor, physx::PxBoxGeometry(1000.0f, 0.01f, 1000.0f), *m_Material);

			m_PhysicsSystem->GetScene()->addActor(*m_BoxActor);
			m_PhysicsSystem->GetScene()->addActor(*m_FloorActor);
			//m_PhysicsSystem->GetScene()->addActor(*m_Car->getRigidDynamicActor());
			

		}

		return true;
	}

	bool TempSceneThree::InitScene()
	{
		
		if (m_EnablePhysics && m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"))
		{
			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"));

			CVector3 vectPos;
			vectPos.x = m_BoxActor->getGlobalPose().p.x;
			vectPos.y = m_BoxActor->getGlobalPose().p.y;
			vectPos.z = m_BoxActor->getGlobalPose().p.z;
			comp->SetPosition(vectPos);

			CVector3 vectRot;
			vectRot.x = m_BoxActor->getGlobalPose().q.x;
			vectRot.y = m_BoxActor->getGlobalPose().q.y;
			vectRot.z = m_BoxActor->getGlobalPose().q.z;
			comp->SetRotation(vectRot);
			
		}
		if (m_EnablePhysics && m_EntityManager->GetEntity("Car")->GetComponent("Transform"))
		{
			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Car")->GetComponent("Transform"));

			/*CVector3 vect;
			vect.x = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().p.x;
			vect.y = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().p.y;
			vect.z = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().p.z;
			comp->SetPosition(vect);
			CVector3 vectRot;
			vectRot.x = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().q.x;
			vectRot.y = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().q.y;
			vectRot.z = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().q.z;
			comp->SetRotation(vectRot);*/
		}
		
		

		m_SceneCamera->SetPosition({ 0, 10, -40 });
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
			m_PhysicsSystem->GetScene()->fetchResults(true);

			if (m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"))
			{

				TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"));
				CVector3 vect;
				vect.x = m_BoxActor->getGlobalPose().p.x;
				vect.y = m_BoxActor->getGlobalPose().p.y;
				vect.z = m_BoxActor->getGlobalPose().p.z;
				comp->SetPosition(vect);
				CVector3 vectRot;
				vectRot.x = m_BoxActor->getGlobalPose().q.x;
				vectRot.y = m_BoxActor->getGlobalPose().q.y;
				vectRot.z = m_BoxActor->getGlobalPose().q.z;
				comp->SetRotation(vectRot);

			}
			if (m_EntityManager->GetEntity("Car")->GetComponent("Transform"))
			{

				TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Car")->GetComponent("Transform"));

				/*CVector3 vect;
				vect.x = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().p.x;
				vect.y = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().p.y;
				vect.z = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().p.z;
				comp->SetPosition(vect);
				CVector3 vectRot;
				vectRot.x = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().q.x;
				vectRot.y = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().q.y;
				vectRot.z = m_VehicleDrive4W->getRigidDynamicActor()->getGlobalPose().q.z;
				comp->SetRotation(vectRot);*/
			}
			
			
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

		//if (m_EnablePhysics && m_WheelsSimData != nullptr) m_WheelsSimData->free();
		if (m_EnablePhysics && m_PhysicsSystem != nullptr) physx::PxCloseVehicleSDK();
		
		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();

	}


	//void TempSceneThree::SetupWheelsSimulationData(
	//	const physx::PxVec3& chassisCMOffset, const physx::PxF32 chassisMass, 
	//	physx::PxVehicleWheelsSimData* wheelsSimData)
	//{
	//	m_WheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].x = -m_WheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].x;
	//	m_WheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].x = -m_WheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].x;
	//	m_WheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].y = m_WheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].y;
	//	m_WheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].y = m_WheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].y;

	//	
	//	// Enable the handbrake for the rear wheels only
	//	m_Wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxBrakeTorque = 4000.0f;
	//	m_Wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxBrakeTorque = 4000.0f;
	//	// Enable seteering for the front wheels only
	//	m_Wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = physx::PxPi * 0.3333f;
	//	m_Wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = physx::PxPi * 0.3333f;

	//	// Setup the tires
	//	physx::PxVehicleTireData tires[PX_MAX_NB_WHEELS];
	//	{
	//		for (physx::PxU32 i = 0; i < 4; ++i)
	//		{
	//			tires[i].mType = TIRE_TYPE_NORMAL;
	//		}
	//	}

	//	// Setup the suspensions
	//	physx::PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
	//	{
	//		// Compute the mass supported by each suspension spring
	//		physx::PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
	//		physx::PxVehicleComputeSprungMasses(4, m_WheelCentreOffsets, chassisCMOffset, chassisMass, 1, suspSprungMasses);

	//		// Set the suspension data
	//		for (physx::PxU32 i = 0; i < 4; ++i)
	//		{
	//			suspensions[i].mMaxCompression = 0.3f;
	//			suspensions[i].mMaxDroop = 0.1f;
	//			suspensions[i].mSpringStrength = 35000.0f;
	//			suspensions[i].mSpringDamperRate = 4500.0f;
	//			suspensions[i].mSprungMass = suspSprungMasses[i];
	//		}

	//		// Set the camber angles
	//		const physx::PxF32 camberAngleAtRest = 0.0f;
	//		const physx::PxF32 camberAngleAtMaxDroop = 0.01f;
	//		const physx::PxF32 camberAngleAtMaxCompression = 0.1f;

	//		for (physx::PxU32 i = 0; i < 4; i += 2)
	//		{
	//			suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
	//			suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
	//			suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtRest;
	//			suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtRest;
	//			suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtRest;
	//			suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtRest;
	//		}
	//	}

	//	// Setup the wheel geometry
	//	physx::PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
	//	physx::PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
	//	physx::PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
	//	physx::PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
	//	{
	//		for (physx::PxU32 i = 0; i < 4; i++)
	//		{
	//			// Vertical suspension Travel
	//			suspTravelDirections[i] = physx::PxVec3(0, -1, 0);

	//			// Wheel centre offset is Offset from rigid body centre of mass
	//			wheelCentreCMOffsets[i] = m_WheelCentreOffsets[i] - chassisCMOffset;

	//			// Suspension force application point 0.3 metres below rigid body centre of mass
	//			suspForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

	//			// Tire Force application point 0.3 metres below rigid body centre of mass
	//			tireForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
	//		}
	//	}

	//	physx::PxFilterData qryFilterData;
	//	SetupNonDrivableSurface(qryFilterData);

	//	// Set the wheel, tire and suspension data
	//	// Set the geomtry data
	//	// Set the query filter data
	//	for (physx::PxU32 i = 0; i < 4; ++i)
	//	{
	//		wheelsSimData->setWheelData(i, m_Wheels[i]);
	//		wheelsSimData->setTireData(i, tires[i]);
	//		wheelsSimData->setSuspensionData(i, suspensions[i]);
	//		wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
	//		wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
	//		wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
	//		wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
	//	}

	//	//Add a front and rear anti-roll bar
	//	physx::PxVehicleAntiRollBarData barFront;
	//	barFront.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
	//	barFront.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
	//	barFront.mStiffness = 10000.0f;
	//	wheelsSimData->addAntiRollBarData(barFront);
	//	physx::PxVehicleAntiRollBarData barRear;
	//	barRear.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT;
	//	barRear.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
	//	barRear.mStiffness = 10000.0f;
	//	wheelsSimData->addAntiRollBarData(barRear);
	//}

	//physx::PxRigidDynamic* TempSceneThree::CreateVehicleActor(const physx::PxVehicleChassisData& chassisData, 
	//	physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numberWheels,
	//	const physx::PxFilterData& wheelSimFilterData, physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, 
	//	const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData, physx::PxPhysics& physics)
	//{
	//	// We need a rigid body actor for the vehicle
	//	// Don't forget to add the actor to the scene after setting up the associated vehicle
	//	physx::PxRigidDynamic* actor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));

	//	

	//	// Wheels and chassis query filter data
	//	// Optional: cars don't drive on other cars
	//	physx::PxFilterData wheelQryFilterData;
	//	// TODO setupNonDrivableSurface function
	//	physx::PxFilterData chassisQryFilterData;
	//	// setupNonDrivableSurface function

	//	// Add all the wheels shape to the actor.
	//	for (physx::PxU32 i = 0; i < numberWheels; ++i)
	//	{
	//		physx::PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
	//		physx::PxShape* wheelShape = physx::PxRigidActorExt::createExclusiveShape(*actor, geom, *wheelMaterials[i]);
	//		wheelShape->setQueryFilterData(wheelQryFilterData);
	//		wheelShape->setSimulationFilterData(wheelSimFilterData);
	//		wheelShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
	//	}

	//	// Add the chassis shapes to the actor
	//	for (physx::PxU32 i = 0; i < numChassisMeshes; ++i)
	//	{
	//		physx::PxShape* chassisShape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxConvexMeshGeometry(chassisConvexMeshes[i]), 
	//			*chassisMaterials[i]);
	//		chassisShape->setQueryFilterData(chassisQryFilterData);
	//		chassisShape->setSimulationFilterData(chassisSimFilterData);
	//		chassisShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
	//	}

	//	actor->setMass(chassisData.mMass);
	//	actor->setMassSpaceInertiaTensor(chassisData.mMOI);
	//	actor->setCMassLocalPose(physx::PxTransform(chassisData.mCMOffset, physx::PxQuat(physx::PxIdentity)));


	//	return actor;
	//}

	//physx::PxVehicleDrive4W* TempSceneThree::CreateVehicle4W(const VehicleDesc& vehicle4WDesc, physx::PxPhysics* physics, physx::PxCooking* cooking)
	//{
	//	const physx::PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
	//	const physx::PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

	//	// Construct a physx actor with shapes for the chassis and wheels
	//	// Set the rigid body mass, moment of inertia and centre of mass offset
	//	physx::PxRigidDynamic* veh4Actor = NULL;
	//	{
	//		// Construct a convex mesh for a cylindrical wheel
	//		//physx::PxConvexMesh* wheelMesh = 
	//		// Assume all wheels are identical for simplicity
	//		physx::PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
	//		physx::PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

	//		physx::PxF32 wheelWidths[4];
	//		physx::PxF32 wheelRadii[4];

	//		// Set the meshes and materials for driven wheels
	//		for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT; ++i)
	//		{
	//			wheelConvexMeshes[i] = CreateWheelMesh(i, *physics, *cooking);

	//			wheelMaterials[i] = m_Material;
	//			
	//		}

	//		MakeWheelWithsAndRadii(wheelConvexMeshes, wheelWidths, wheelRadii);
	//		physx::PxF32 wheelMOIs[4];
	//		physx::PxF32 wheelMass = 20.0f;
	//		
	//		for (physx::PxU32 i = 0; i < 4; ++i)
	//		{
	//			wheelMOIs[i] = 0.5f * wheelMass * wheelRadii[i] * wheelRadii[i];
	//		}

	//		for (physx::PxU32 i = 0; i < 4; ++i)
	//		{
	//			m_Wheels[i].mRadius = wheelRadii[i];
	//			m_Wheels[i].mMass = wheelMass;
	//			m_Wheels[i].mMOI = wheelMOIs[i];
	//			m_Wheels[i].mWidth = wheelWidths[i];
	//		}


	//		// Chassis just has a single convex shape for simplicity 
	//		physx::PxConvexMesh* chassisConvexMesh = CreateChassisMesh(4, *physics, *cooking);
	//		physx::PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
	//		physx::PxMaterial* chassisMaterials[1] = { m_Material };
	//		physx::PxVec3 chassisDims = MakeChassis(chassisConvexMesh);
	//		m_ChassisDims = chassisDims;

	//		//The origin is at the center of the chassis mesh.
	//		//Set the center of mass to be below this point and a little towards the front.
	//		m_ChassisData.mCMOffset = physx::PxVec3(0.0f, -chassisDims.y * 0.5f + 0.65, 0.25f);

	//		//Now compute the chassis mass and moment of inertia.
	//		//Use the moment of inertia of a cuboid as an approximate value for the chassis moi.
	//		physx::PxVec3 chassisMOI
	//		((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * vehicle4WDesc.chassisMass / 12.0f,
	//			(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * vehicle4WDesc.chassisMass / 12.0f,
	//			(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * vehicle4WDesc.chassisMass / 12.0f);

	//		//A bit of tweaking here.  The car will have more responsive turning if we reduce the 	
	//		//y-component of the chassis moment of inertia.
	//		chassisMOI.y *= 0.8f;
	//		m_ChassisData.mMOI = chassisMOI;


	//		veh4Actor = CreateVehicleActor(m_ChassisData,
	//			wheelMaterials, wheelConvexMeshes, 4, wheelSimFilterData,
	//			chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData,
	//			*physics);

	//		

	//	}

	//	// Setup the sim data for the wheels
	//	physx::PxVehicleWheelsSimData* wheelsSimData = physx::PxVehicleWheelsSimData::allocate(4);
	//	{
	//		SetupWheelsSimulationData(m_ChassisData.mCMOffset, m_ChassisData.mMass, wheelsSimData);
	//	}

	//	// Setup sim data for the vehicle drive model
	//	physx::PxVehicleDriveSimData4W driveSimData;
	//	{
	//		// Diff
	//		physx::PxVehicleDifferential4WData diff;
	//		diff.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
	//		driveSimData.setDiffData(diff);

	//		// Engine
	//		physx::PxVehicleEngineData engine;
	//		engine.mPeakTorque = 500.0f;
	//		engine.mMaxOmega = 600.0f; //Approx 6000rpm
	//		driveSimData.setEngineData(engine);

	//		// Gears
	//		physx::PxVehicleGearsData gears;
	//		gears.mSwitchTime = 0.5f;
	//		driveSimData.setGearsData(gears);

	//		// Clutch
	//		physx::PxVehicleClutchData clutch;
	//		clutch.mStrength = 10.0f;
	//		driveSimData.setClutchData(clutch);

	//		// Ackerman steer accuracy
	//		physx::PxVehicleAckermannGeometryData ackermann;
	//		ackermann.mAccuracy = 1.0f;
	//		ackermann.mAxleSeparation =
	//			wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
	//			wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
	//		ackermann.mFrontWidth =
	//			wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
	//			wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
	//		ackermann.mRearWidth =
	//			wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
	//			wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
	//		driveSimData.setAckermannGeometryData(ackermann);
	//	}

	//	// Create a vehicle from the wheels and drive sim data;
	//	physx::PxVehicleDrive4W* vehDrive4W = physx::PxVehicleDrive4W::allocate(4);

	//
	//	vehDrive4W->setup(physics, veh4Actor, *wheelsSimData, driveSimData, 4 - 4);
	//	vehDrive4W->mWheelsSimData.setWheelShapeMapping(0, 0);
	//	vehDrive4W->mWheelsSimData.setWheelShapeMapping(1, 1);
	//	vehDrive4W->mWheelsSimData.setWheelShapeMapping(2, 2);
	//	vehDrive4W->mWheelsSimData.setWheelShapeMapping(3, 3);
	//	
	//	physx::PxFilterData vehQryFilterData;
	//	SetupNonDrivableSurface(vehQryFilterData);
	//	vehDrive4W->mWheelsSimData.setSceneQueryFilterData(0, vehQryFilterData);
	//	vehDrive4W->mWheelsSimData.setSceneQueryFilterData(1, vehQryFilterData);
	//	vehDrive4W->mWheelsSimData.setSceneQueryFilterData(2, vehQryFilterData);
	//	vehDrive4W->mWheelsSimData.setSceneQueryFilterData(3, vehQryFilterData);

	//	vehDrive4W->getRigidDynamicActor()->setGlobalPose({ 0, 0, 0 });


	//	// Free the sim data because its not needed anymore
	//	wheelsSimData->free();

	//	return vehDrive4W;
	//}

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

	physx::PxConvexMesh* TempSceneThree::CreateWheelMesh(int index, physx::PxPhysics& physics, physx::PxCooking& cooking)
	{
		physx::PxU32 vertexCount;
		std::vector<physx::PxVec3> vertices;
		if (m_EntityManager->GetEntity("Car")->GetComponent("Renderer"))
		{
			RendererComponent* comp = static_cast<RendererComponent*>(m_EntityManager->GetEntity("Car")->GetComponent("Renderer"));
			vertexCount = comp->GetNumberOfVertices(index);

			std::vector<CVector3> nChassis = comp->GetVertices(index);

			for (int i = 0; i < vertexCount; ++i)
			{
				vertices.push_back({ nChassis[i].x,  nChassis[i].y, nChassis[i].z });
			}
		}

		physx::PxVec3* v = vertices.data();


		return CreateConvexMesh(v, vertexCount, physics, cooking);
	}

	physx::PxConvexMesh* TempSceneThree::CreateChassisMesh(int index, physx::PxPhysics& physics, physx::PxCooking& cooking)
	{
		physx::PxU32 vertexCount;
		std::vector<physx::PxVec3> vertices;
		if (m_EntityManager->GetEntity("Car")->GetComponent("Renderer"))
		{
			RendererComponent* comp = static_cast<RendererComponent*>(m_EntityManager->GetEntity("Car")->GetComponent("Renderer"));
			vertexCount = comp->GetNumberOfVertices(index);

			std::vector<CVector3> Chassis = comp->GetVertices(index);
			
			for (int i = 0; i < vertexCount; ++i)
			{
				vertices.push_back({ Chassis[i].x,  Chassis[i].y, Chassis[i].z });
			}
		}

		physx::PxVec3* v = vertices.data();

		return CreateConvexMesh(v, vertexCount, physics, cooking);
	}

	void TempSceneThree::MakeWheelWithsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii)
	{
		for (physx::PxU32 i = 0; i < 4; i++)
		{
			const physx::PxU32 numWheelVerts = wheelConvexMeshes[i]->getNbVertices();
			const physx::PxVec3* wheelVerts = wheelConvexMeshes[i]->getVertices();
			physx::PxVec3 wheelMin(PX_MAX_F32, PX_MAX_F32, PX_MAX_F32);
			physx::PxVec3 wheelMax(-PX_MAX_F32, -PX_MAX_F32, -PX_MAX_F32);
			for (physx::PxU32 j = 0; j < numWheelVerts; j++)
			{
				wheelMin.x = physx::PxMin(wheelMin.x, wheelVerts[j].x);
				wheelMin.y = physx::PxMin(wheelMin.y, wheelVerts[j].y);
				wheelMin.z = physx::PxMin(wheelMin.z, wheelVerts[j].z);
				wheelMax.x = physx::PxMax(wheelMax.x, wheelVerts[j].x);
				wheelMax.y = physx::PxMax(wheelMax.y, wheelVerts[j].y);
				wheelMax.z = physx::PxMax(wheelMax.z, wheelVerts[j].z);
			}
			wheelWidths[i] = wheelMax.x - wheelMin.x;
			wheelRadii[i] = physx::PxMax(wheelMax.y, wheelMax.z) * 0.975f;
		}
	}

	physx::PxVehicleDrive4W* TempSceneThree::Create4WVehicle()
	{
		physx::PxVehicleWheelsSimData* wheelSimData = physx::PxVehicleWheelsSimData::allocate(4);
		physx::PxVehicleDriveSimData4W driveSimData;
		physx::PxVehicleChassisData chassisData;

		m_ChassisConvexMesh = CreateChassisMesh(4, *m_PhysicsSystem->GetPhysics(), *m_PhysicsSystem->GetCooking());

		m_WheelConvexMeshes[0] = CreateWheelMesh(0, *m_PhysicsSystem->GetPhysics(), *m_PhysicsSystem->GetCooking());
		m_WheelConvexMeshes[1] = CreateWheelMesh(1, *m_PhysicsSystem->GetPhysics(), *m_PhysicsSystem->GetCooking());
		m_WheelConvexMeshes[2] = CreateWheelMesh(2, *m_PhysicsSystem->GetPhysics(), *m_PhysicsSystem->GetCooking());
		m_WheelConvexMeshes[3] = CreateWheelMesh(3, *m_PhysicsSystem->GetPhysics(), *m_PhysicsSystem->GetCooking());

		
		m_WheelCentreOffsets4[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].x = -m_WheelCentreOffsets4[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].x;
		m_WheelCentreOffsets4[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].x = -m_WheelCentreOffsets4[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].x;
		m_WheelCentreOffsets4[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].y = m_WheelCentreOffsets4[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].y;
		m_WheelCentreOffsets4[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].y = m_WheelCentreOffsets4[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].y;
		physx::PxF32 wheelOffsetZ = 0;
		for (auto i = 0; i < 4; ++i)
		{
			wheelOffsetZ += m_WheelCentreOffsets4[i].z;
		}
		wheelOffsetZ *= 0.25f;
		//gChassisMeshTransform.z += wheelOffsetZ;

		CreateVehicle4WSimulationData(1500.0f, chassisData, m_WheelCentreOffsets4, m_WheelConvexMeshes, 20.0f, *wheelSimData, driveSimData);

		physx::PxRigidDynamic* vehActor = CreateVehicleActor4W(m_WheelConvexMeshes, m_ChassisConvexMesh, chassisData);

		physx::PxVehicleDrive4W* car = physx::PxVehicleDrive4W::allocate(4);
		car->setup(m_PhysicsSystem->GetPhysics(), vehActor, *wheelSimData, driveSimData, 0);

		wheelSimData->free();

		car->mWheelsSimData.setWheelShapeMapping(0, 0);
		car->mWheelsSimData.setWheelShapeMapping(1, 1);
		car->mWheelsSimData.setWheelShapeMapping(2, 2);
		car->mWheelsSimData.setWheelShapeMapping(3, 3);

		physx::PxFilterData vehQryFilterData;
		//SampleVehicleSetupVehicleShapeQueryFilterData(&vehQryFilterData);
		car->mWheelsSimData.setSceneQueryFilterData(0, vehQryFilterData);
		car->mWheelsSimData.setSceneQueryFilterData(1, vehQryFilterData);
		car->mWheelsSimData.setSceneQueryFilterData(2, vehQryFilterData);
		car->mWheelsSimData.setSceneQueryFilterData(3, vehQryFilterData);

		car->mDriveDynData.setUseAutoGears(true);

		return car;
	}

	void TempSceneThree::CreateVehicle4WSimulationData(physx::PxF32 chassisMass, physx::PxVehicleChassisData& chassisData, 
		physx::PxVec3* wheelCentreOffsets, physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32 wheelMass, physx::PxVehicleWheelsSimData& wheelsData,
		physx::PxVehicleDriveSimData4W& driveData)
	{
		physx::PxVec3 chassisDims = MakeChassis(m_ChassisConvexMesh);

		const physx::PxVec3 chassisCMOffSet = physx::PxVec3(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

		physx::PxVec3 chassisMOI((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
								 (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
								 (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);

		chassisMOI.y *= 0.8f;

		chassisData.mMass = chassisMass;
		chassisData.mMOI = chassisMOI;
		chassisData.mCMOffset = chassisCMOffSet;

		physx::PxF32 suspSprungMasses[4];
		physx::PxVehicleComputeSprungMasses(4, wheelCentreOffsets, chassisCMOffSet, chassisMass, 1, suspSprungMasses);

		physx::PxF32 wheelWidth[4], wheelRadii[4];
		MakeWheelWithsAndRadii(wheelConvexMeshes, wheelWidth, wheelRadii);

		physx::PxF32 wheelMOIs[4];
		for (auto i = 0; i < 4; ++i)
		{
			wheelMOIs[i] = 0.5f * wheelMass * wheelRadii[i] * wheelRadii[i];
		}

		physx::PxVehicleWheelData wheels[4];
		for (auto i = 0; i < 4; ++i)
		{
			wheels[i].mRadius = wheelRadii[i];
			wheels[i].mMass = wheelMass;
			wheels[i].mMOI = wheelMOIs[i];
			wheels[i].mWidth = wheelWidth[i];
		}

		// Disable handbrake for front wheels and enable for rear wheels
		wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxHandBrakeTorque = 0.0f;
		wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxHandBrakeTorque = 0.0f;
		wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
		wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
		// Enable steering for the front wheels and disable for the rear wheels
		wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = physx::PxPi* 0.3333f;
		wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = physx::PxPi * 0.3333f;
		wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxSteer = 0.0f;
		wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxSteer = 0.0f;

		physx::PxVehicleTireData tires[4];
		tires[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mType = TIRE_TYPE_SLICKS;
		tires[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mType = TIRE_TYPE_SLICKS;
		tires[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mType = TIRE_TYPE_SLICKS;
		tires[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mType = TIRE_TYPE_SLICKS;

		physx::PxVehicleSuspensionData susps[4];
		for (auto i = 0; i < 4; ++i)
		{
			susps[i].mMaxCompression = 0.3f;
			susps[i].mMaxDroop = 0.1f;
			susps[i].mSpringStrength = 35000.0f;
			susps[i].mSpringDamperRate = 4500.0f;
		}
		susps[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mSprungMass = suspSprungMasses[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT];
		susps[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mSprungMass = suspSprungMasses[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT];
		susps[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mSprungMass = suspSprungMasses[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT];
		susps[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mSprungMass = suspSprungMasses[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT];

		const physx::PxF32 camberAngleAtRest = 0.0f;
		susps[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mCamberAtRest = camberAngleAtRest;
		susps[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mCamberAtRest = -camberAngleAtRest;
		susps[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mCamberAtRest = camberAngleAtRest;
		susps[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mCamberAtRest = -camberAngleAtRest;

		const physx::PxF32 camberAngleAtMaxDroop = 0.001f;
		susps[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mCamberAtMaxDroop = camberAngleAtMaxDroop;
		susps[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
		susps[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mCamberAtMaxDroop = camberAngleAtMaxDroop;
		susps[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mCamberAtMaxDroop = -camberAngleAtMaxDroop;

		const physx::PxF32 camberAngleAtMaxCompression = -0.001f;
		susps[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mCamberAtMaxCompression = camberAngleAtMaxCompression;
		susps[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
		susps[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mCamberAtMaxCompression = camberAngleAtMaxCompression;
		susps[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mCamberAtMaxCompression = -camberAngleAtMaxCompression;

		physx::PxVec3 suspTravelDirections[4] = { physx::PxVec3(0,-1,0),physx::PxVec3(0,-1,0),physx::PxVec3(0,-1,0),physx::PxVec3(0,-1,0) };
		physx::PxVec3 wheelCentreCMOffsets[4];
		physx::PxVec3 suspForceAppCMOffsets[4];
		physx::PxVec3 tireForceAppCMOffsets[4];

		for (auto i = 0; i < 4; ++i)
		{
			wheelCentreCMOffsets[i] = wheelCentreOffsets[i] - chassisCMOffSet;
			suspForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
			tireForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
		}

		for (auto i = 0; i < 4; ++i)
		{
			wheelsData.setWheelData(i, wheels[i]);
			wheelsData.setTireData(i, tires[i]);
			wheelsData.setSuspensionData(i, susps[i]);
			wheelsData.setSuspTravelDirection(i, suspTravelDirections[i]);
			wheelsData.setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
			wheelsData.setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
			wheelsData.setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
		}

		//Set the car to perform 3 sub-steps when it moves with a forwards speed of less than 5.0 
		//and with a single step when it moves at speed greater than or equal to 5.0.
		wheelsData.setSubStepCount(5.0f, 3, 1);

		//Diff
		physx::PxVehicleDifferential4WData diff;
		diff.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
		driveData.setDiffData(diff);

		//Engine
		physx::PxVehicleEngineData engine;
		engine.mPeakTorque = 500.0f;
		engine.mMaxOmega = 600.0f;//approx 6000 rpm
		driveData.setEngineData(engine);

		//Gears
		physx::PxVehicleGearsData gears;
		gears.mSwitchTime = 0.5f;
		driveData.setGearsData(gears);

		//Clutch
		physx::PxVehicleClutchData clutch;
		clutch.mStrength = 10.0f;
		driveData.setClutchData(clutch);

		//Ackermann steer accuracy
		physx::PxVehicleAckermannGeometryData ackermann;
		ackermann.mAccuracy = 1.0f;
		ackermann.mAxleSeparation = wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].z - wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].z;
		ackermann.mFrontWidth = wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].x - wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].x;
		ackermann.mRearWidth = wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].x - wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].x;
		driveData.setAckermannGeometryData(ackermann);
		
	}

	physx::PxRigidDynamic* TempSceneThree::CreateVehicleActor4W(physx::PxConvexMesh** wheelConvexMeshes, physx::PxConvexMesh* chassisConvexMesh, 
		physx::PxVehicleChassisData& chassisData)
	{
		physx::PxRigidDynamic* vehActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));

		physx::PxConvexMeshGeometry frontLeftWheelGeom(wheelConvexMeshes[0]);
		physx::PxConvexMeshGeometry frontRightWheelGeom(wheelConvexMeshes[1]);
		physx::PxConvexMeshGeometry rearLeftWheelGeom(wheelConvexMeshes[2]);
		physx::PxConvexMeshGeometry rearRightWheelGeom(wheelConvexMeshes[3]);

		physx::PxGeometry* wheelGeomentries[4] = { &frontLeftWheelGeom, &frontRightWheelGeom, &rearLeftWheelGeom, &rearRightWheelGeom };
		physx::PxTransform wheelLocalPoses[4] = { physx::PxTransform(physx::PxIdentity),physx::PxTransform(physx::PxIdentity),physx::PxTransform(physx::PxIdentity),physx::PxTransform(physx::PxIdentity) };
		physx::PxFilterData wheelCollFilterData;
		wheelCollFilterData.word0 = COLLISION_FLAG_WHEEL;
		wheelCollFilterData.word1 = COLLISION_FLAG_WHEEL_AGAINST;

		physx::PxConvexMeshGeometry chassisConvexGeom(chassisConvexMesh);
		physx::PxGeometry* chassisGeom[1] = { &chassisConvexGeom };
		physx::PxTransform chassisLocalPoses[1] = { physx::PxTransform(physx::PxIdentity) };
		physx::PxFilterData chassisCollFilterData;
		chassisCollFilterData.word0 = COLLISION_FLAG_CHASSIS;
		chassisCollFilterData.word1 = COLLISION_FLAG_CHASSIS_AGAINST;

		// TODO
		//Create a query filter data for the car to ensure that cars
		//do not attempt to drive on themselves.
		physx::PxFilterData vehQryFilterData;
		//SampleVehicleSetupVehicleShapeQueryFilterData(&vehQryFilterData);
		SetupActor(vehActor, wheelGeomentries, vehQryFilterData, wheelCollFilterData, wheelLocalPoses, chassisGeom, chassisCollFilterData, chassisLocalPoses, chassisData);

		return vehActor;
		
	}

	void TempSceneThree::SetupActor(physx::PxRigidDynamic* vehActor, physx::PxGeometry** wheelGeometries, physx::PxFilterData& vehQryFilterData, 
		physx::PxFilterData& wheelCollFilterData, physx::PxTransform* wheelLocalPoses, physx::PxGeometry** chassisGeometries, 
		physx::PxFilterData& chassisCollFilterData, physx::PxTransform* chassisLocalPoses, physx::PxVehicleChassisData& chassisData)
	{
		for (auto i = 0; i < 4; ++i)
		{
			physx::PxShape* wheelShape = physx::PxRigidActorExt::createExclusiveShape(*vehActor, *wheelGeometries[i], *m_Material);
			wheelShape->setQueryFilterData(vehQryFilterData);
			wheelShape->setSimulationFilterData(wheelCollFilterData);
			wheelShape->setLocalPose(wheelLocalPoses[i]);
		}

		physx::PxShape* chassisShape = physx::PxRigidActorExt::createExclusiveShape(*vehActor, *chassisGeometries[0], *m_Material);
		chassisShape->setQueryFilterData(vehQryFilterData);
		chassisShape->setSimulationFilterData(chassisCollFilterData);
		chassisShape->setLocalPose(chassisLocalPoses[0]);

		vehActor->setMass(chassisData.mMass);
		vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
		vehActor->setCMassLocalPose(physx::PxTransform(chassisData.mCMOffset, physx::PxQuat(physx::PxIdentity)));
	}

	physx::PxVec3 TempSceneThree::MakeChassis(physx::PxConvexMesh* chassisConvexMesh)
	{
		const physx::PxU32 numChassisVerts = chassisConvexMesh->getNbVertices();
		const physx::PxVec3* chassisVerts = chassisConvexMesh->getVertices();
		physx::PxVec3 chassisMin(PX_MAX_F32, PX_MAX_F32, PX_MAX_F32);
		physx::PxVec3 chassisMax(-PX_MAX_F32, -PX_MAX_F32, -PX_MAX_F32);
		for (physx::PxU32 i = 0; i < numChassisVerts; i++)
		{
			chassisMin.x = physx::PxMin(chassisMin.x, chassisVerts[i].x);
			chassisMin.y = physx::PxMin(chassisMin.y, chassisVerts[i].y);
			chassisMin.z = physx::PxMin(chassisMin.z, chassisVerts[i].z);
			chassisMax.x = physx::PxMax(chassisMax.x, chassisVerts[i].x);
			chassisMax.y = physx::PxMax(chassisMax.y, chassisVerts[i].y);
			chassisMax.z = physx::PxMax(chassisMax.z, chassisVerts[i].z);
		}
		const physx::PxVec3 chassisDims = chassisMax - chassisMin;
		return chassisDims;
	}

	/*TempSceneThree::VehicleDesc TempSceneThree::InitVehicleDesc()
	{
		VehicleDesc vehicleDesc;

		
		vehicleDesc.chassisSimFilterData = physx::PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

		vehicleDesc.chassisSimFilterData = physx::PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

		return vehicleDesc;
	}*/

	
}