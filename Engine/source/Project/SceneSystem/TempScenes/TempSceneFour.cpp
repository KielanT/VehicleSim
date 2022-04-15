#include "ppch.h"
#include "TempSceneFour.h"
#include "Project/EntitySystem/Components/TransformComponent.h"


namespace Project
{
	TempSceneFour::TempSceneFour(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour /*= CVector3(1.0f, 1.0f, 1.0f)*/, float specularPower /*= 256.0f*/, ColourRGBA backgroundColour /*= ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f)*/, bool vsyncOn /*= true*/)
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


	TempSceneFour::TempSceneFour(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour /*= CVector3(1.0f, 1.0f, 1.0f)*/, float specularPower /*= 256.0f*/, ColourRGBA backgroundColour /*= ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f)*/, bool vsyncOn /*= true*/)
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

	bool TempSceneFour::InitGeometry()
	{
		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = new Camera();

		std::string path = "media/";
		m_EntityManager->CreateModelEntity("Floor", path + "Ground.x");

		m_EntityManager->CreateModelEntity("Test Cube", path + "Cube.x", path + "brick1.jpg");
		
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

			// Create a basic vehicle
			VehicleDesc vehicleDesc = InitVehicleDesc();
			m_Vehicle4W = CreateVehicle4W(vehicleDesc);
			m_Vehicle4W->getRigidDynamicActor()->setGlobalPose({ 0.0f, 5.0f, 0.0f });

			// Set Actors and shapes here
			m_BoxActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform({ 0.0f, 40.0f, 0.0f }));
			m_BoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor, physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *m_Material);

			m_FloorActor = m_PhysicsSystem->GetPhysics()->createRigidStatic({ 0.0f, 0.0f, 0.0f });
			m_FloorBoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_FloorActor, physx::PxBoxGeometry(1000.0f, 0.01f, 1000.0f), *m_Material);

			m_PhysicsSystem->GetScene()->addActor(*m_BoxActor);
			m_PhysicsSystem->GetScene()->addActor(*m_FloorActor);
			m_PhysicsSystem->GetScene()->addActor(*m_Vehicle4W->getRigidDynamicActor());
		}

		return true;
	}

	bool TempSceneFour::InitScene()
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

		

		
		// Pos 1
		m_SceneCamera->SetPosition({ 0, 10, -40 });
		m_SceneCamera->SetRotation({ 0, 0, 0 });

		// Pos 2
		//m_SceneCamera->SetPosition({ -40, 10, 0 });
		//m_SceneCamera->SetRotation({ 0, ToRadians(90), 0 });

		return true;
	}

	void TempSceneFour::RenderScene()
	{
		m_EntityManager->RenderAllEntities();
	}

	void TempSceneFour::UpdateScene(float frameTime)
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
		}

		m_EntityManager->UpdateAllEntities(frameTime);

		m_SceneCamera->Control(frameTime);
		if (KeyHit(Key_L))
		{
			//m_sceneManager->RemoveSceneAtIndex(0);
			//m_sceneManager->LoadScene(1);
		}
	}

	void TempSceneFour::ReleaseResources()
	{
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)    m_EntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);

		//if (m_EnablePhysics && m_WheelsSimData != nullptr) m_WheelsSimData->free();
		if (m_EnablePhysics && m_PhysicsSystem != nullptr) physx::PxCloseVehicleSDK();

		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

	TempSceneFour::VehicleDesc TempSceneFour::InitVehicleDesc()
	{
		const physx::PxF32 chassisMass = 1500.0f;
		const physx::PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
		const physx::PxVec3 chassisMOI
		((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
			(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
			(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
		const physx::PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

		const physx::PxF32 wheelMass = 20.0f;
		const physx::PxF32 wheelRadius = 0.5f;
		const physx::PxF32 wheelWidth = 0.4f;
		const physx::PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
		const physx::PxU32 nbWheels = 4;

		VehicleDesc vehicleDesc;

		vehicleDesc.chassisMass = chassisMass;
		vehicleDesc.chassisDims = chassisDims;
		vehicleDesc.chassisMOI = chassisMOI;
		vehicleDesc.chassisCMOffset = chassisCMOffset;
		vehicleDesc.chassisMaterial = m_Material;
		vehicleDesc.chassisSimFilterData = physx::PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

		vehicleDesc.wheelMass = wheelMass;
		vehicleDesc.wheelRadius = wheelRadius;
		vehicleDesc.wheelWidth = wheelWidth;
		vehicleDesc.wheelMOI = wheelMOI;
		vehicleDesc.numWheels = nbWheels;
		vehicleDesc.wheelMaterial = m_Material;
		vehicleDesc.chassisSimFilterData = physx::PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

		return vehicleDesc;
	}

	physx::PxVehicleDrive4W* TempSceneFour::CreateVehicle4W(const VehicleDesc& vehicle4WDesc)
	{
		const physx::PxVec3 chassisDims = vehicle4WDesc.chassisDims;
		const physx::PxF32 wheelWidth = vehicle4WDesc.wheelWidth;
		const physx::PxF32 wheelRadius = vehicle4WDesc.wheelRadius;
		const physx::PxU32 numWheels = vehicle4WDesc.numWheels;

		const physx::PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
		const physx::PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

		//Construct a physx actor with shapes for the chassis and wheels.
		//Set the rigid body mass, moment of inertia, and center of mass offset.
		physx::PxRigidDynamic* veh4WActor = NULL;
		{
			//Construct a convex mesh for a cylindrical wheel.
			physx::PxConvexMesh* wheelMesh[4];
			for (int i = 0; i < 4; ++i) 
			{
				wheelMesh[i] = CreateWheelMesh(i, wheelWidth, wheelRadius);
			}
			//Assume all wheels are identical for simplicity.
			physx::PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
			physx::PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

			//Set the meshes and materials for the driven wheels.
			for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
			{
				wheelConvexMeshes[i] = wheelMesh[i];
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}
			//Set the meshes and materials for the non-driven wheels
			for (auto i = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
			{
				wheelConvexMeshes[i] = wheelMesh[i];
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}

			//Chassis just has a single convex shape for simplicity.
			physx::PxConvexMesh* chassisConvexMesh = CreateChassisMesh(4);
			physx::PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
			physx::PxMaterial* chassisMaterials[1] = { vehicle4WDesc.chassisMaterial };

			//Rigid body data.
			physx::PxVehicleChassisData rigidBodyData;
			rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
			rigidBodyData.mMass = vehicle4WDesc.chassisMass;
			rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

			veh4WActor = CreateVehicleActor
			(rigidBodyData,
				wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData,
				chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData);
		}

		//Set up the sim data for the wheels.
		physx::PxVehicleWheelsSimData* wheelsSimData = physx::PxVehicleWheelsSimData::allocate(numWheels);
		{
			//Compute the wheel center offsets from the origin.
			physx::PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
			const physx::PxF32 frontZ = chassisDims.z * 0.3f;
			const physx::PxF32 rearZ = -chassisDims.z * 0.3f;
			ComputeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels, wheelCenterActorOffsets);

			//Set up the simulation data for all wheels.
			SetupWheelsSimulationData
			(vehicle4WDesc.wheelMass, vehicle4WDesc.wheelMOI, wheelRadius, wheelWidth,
				numWheels, wheelCenterActorOffsets,
				vehicle4WDesc.chassisCMOffset, vehicle4WDesc.chassisMass,
				wheelsSimData);
		}

		//Set up the sim data for the vehicle drive model.
		physx::PxVehicleDriveSimData4W driveSimData;
		{
			//Diff
			physx::PxVehicleDifferential4WData diff;
			diff.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
			driveSimData.setDiffData(diff);

			//Engine
			physx::PxVehicleEngineData engine;
			engine.mPeakTorque = 500.0f;
			engine.mMaxOmega = 600.0f;//approx 6000 rpm
			driveSimData.setEngineData(engine);

			//Gears
			physx::PxVehicleGearsData gears;
			gears.mSwitchTime = 0.5f;
			driveSimData.setGearsData(gears);

			//Clutch
			physx::PxVehicleClutchData clutch;
			clutch.mStrength = 10.0f;
			driveSimData.setClutchData(clutch);

			//Ackermann steer accuracy
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

		//Create a vehicle from the wheels and drive sim data.
		physx::PxVehicleDrive4W* vehDrive4W = physx::PxVehicleDrive4W::allocate(numWheels);
		vehDrive4W->setup(m_PhysicsSystem->GetPhysics(), veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);

		////Configure the userdata
		//ConfigureUserData(vehDrive4W, vehicle4WDesc.actorUserData, vehicle4WDesc.shapeUserDatas);

		//Free the sim data because we don't need that any more.
		wheelsSimData->free();

		return vehDrive4W;
	}

	physx::PxConvexMesh* TempSceneFour::CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts)
	{
		// Create descriptor for convex mesh
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = numVerts;
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = verts;
		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxConvexMesh* convexMesh = NULL;
		physx::PxDefaultMemoryOutputStream buf;
		if (m_PhysicsSystem->GetCooking()->cookConvexMesh(convexDesc, buf))
		{
			physx::PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
			convexMesh = m_PhysicsSystem->GetPhysics()->createConvexMesh(id);
		}

		return convexMesh;
	}

	physx::PxConvexMesh* TempSceneFour::CreateWheelMesh(int index, const physx::PxF32 width, const physx::PxF32 radius)
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

		return CreateConvexMesh(v, vertexCount);
	}

	physx::PxConvexMesh* TempSceneFour::CreateChassisMesh(int index)
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

		return CreateConvexMesh(v, vertexCount);
	}

	physx::PxRigidDynamic* TempSceneFour::CreateVehicleActor(const physx::PxVehicleChassisData& chassisData, physx::PxMaterial** wheelMaterials, 
		physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, const physx::PxFilterData& wheelSimFilterData, 
		physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, 
		const physx::PxFilterData& chassisSimFilterData)
	{
		//We need a rigid body actor for the vehicle.
		//Don't forget to add the actor to the scene after setting up the associated vehicle.
		physx::PxRigidDynamic* vehActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));

		//Wheel and chassis query filter data.
		//Optional: cars don't drive on other cars.
		physx::PxFilterData wheelQryFilterData;
		setupNonDrivableSurface(wheelQryFilterData);
		physx::PxFilterData chassisQryFilterData;
		setupNonDrivableSurface(chassisQryFilterData);

		//Add all the wheel shapes to the actor.
		for (auto i = 0; i < numWheels; i++)
		{
			physx::PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
			physx::PxShape* wheelShape = physx::PxRigidActorExt::createExclusiveShape(*vehActor, geom, *wheelMaterials[i]);
			wheelShape->setQueryFilterData(wheelQryFilterData);
			wheelShape->setSimulationFilterData(wheelSimFilterData);
			wheelShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		}

		//Add the chassis shapes to the actor.
		for (auto i = 0; i < numChassisMeshes; i++)
		{
			physx::PxShape* chassisShape = physx::PxRigidActorExt::createExclusiveShape(*vehActor, physx::PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]);
			chassisShape->setQueryFilterData(chassisQryFilterData);
			chassisShape->setSimulationFilterData(chassisSimFilterData);
			chassisShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		}

		vehActor->setMass(chassisData.mMass);
		vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
		vehActor->setCMassLocalPose(physx::PxTransform(chassisData.mCMOffset, physx::PxQuat(physx::PxIdentity)));

		return vehActor;
	}

	void TempSceneFour::ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, 
		const physx::PxF32 wheelWidth, const physx::PxF32 wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets)
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
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1) * deltaZ);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1) * deltaZ);
		//Set the remaining wheels.
		for (auto i = 2, wheelCount = 4; i < numWheels - 2; i += 2, wheelCount += 2)
		{
			wheelCentreOffsets[wheelCount + 0] = physx::PxVec3((-chassisDims.x + wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
			wheelCentreOffsets[wheelCount + 1] = physx::PxVec3((+chassisDims.x - wheelWidth) * 0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
		}
	}

	void TempSceneFour::SetupWheelsSimulationData(const physx::PxF32 wheelMass, const physx::PxF32 wheelMOI, const physx::PxF32 wheelRadius, 
		const physx::PxF32 wheelWidth, const physx::PxU32 numWheels, const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset, 
		const physx::PxF32 chassisMass, physx::PxVehicleWheelsSimData* wheelsSimData)
	{
		//Set up the wheels.
		physx::PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
		{
			//Set up the wheel data structures with mass, moi, radius, width.
			for (auto i = 0; i < numWheels; i++)
			{
				wheels[i].mMass = wheelMass;
				wheels[i].mMOI = wheelMOI;
				wheels[i].mRadius = wheelRadius;
				wheels[i].mWidth = wheelWidth;
			}

			//Enable the handbrake for the rear wheels only.
			wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
			wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
			//Enable steering for the front wheels only.
			wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = physx::PxPi * 0.3333f;
			wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = physx::PxPi * 0.3333f;
		}

		//Set up the tires.
		physx::PxVehicleTireData tires[PX_MAX_NB_WHEELS];
		{
			//Set up the tires.
			for (auto i = 0; i < numWheels; i++)
			{
				tires[i].mType = TIRE_TYPE_NORMAL;
			}
		}

		//Set up the suspensions
		physx::PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
		{
			//Compute the mass supported by each suspension spring.
			physx::PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
			PxVehicleComputeSprungMasses
			(numWheels, wheelCenterActorOffsets,
				chassisCMOffset, chassisMass, 1, suspSprungMasses);

			//Set the suspension data.
			for (auto i = 0; i < numWheels; i++)
			{
				suspensions[i].mMaxCompression = 0.3f;
				suspensions[i].mMaxDroop = 0.1f;
				suspensions[i].mSpringStrength = 35000.0f;
				suspensions[i].mSpringDamperRate = 4500.0f;
				suspensions[i].mSprungMass = suspSprungMasses[i];
			}

			//Set the camber angles.
			const physx::PxF32 camberAngleAtRest = 0.0;
			const physx::PxF32 camberAngleAtMaxDroop = 0.01f;
			const physx::PxF32 camberAngleAtMaxCompression = -0.01f;
			for (auto i = 0; i < numWheels; i += 2)
			{
				suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
				suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
				suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
				suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
				suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
				suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
			}
		}

		//Set up the wheel geometry.
		physx::PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
		physx::PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
		physx::PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
		physx::PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
		{
			//Set the geometry data.
			for (auto i = 0; i < numWheels; i++)
			{
				//Vertical suspension travel.
				suspTravelDirections[i] = physx::PxVec3(0, -1, 0);

				//Wheel center offset is offset from rigid body center of mass.
				wheelCentreCMOffsets[i] =
					wheelCenterActorOffsets[i] - chassisCMOffset;

				//Suspension force application point 0.3 metres below 
				//rigid body center of mass.
				suspForceAppCMOffsets[i] =
					physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

				//Tire force application point 0.3 metres below 
				//rigid body center of mass.
				tireForceAppCMOffsets[i] =
					physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
			}
		}

		//Set up the filter data of the raycast that will be issued by each suspension.
		physx::PxFilterData qryFilterData;
		setupNonDrivableSurface(qryFilterData);

		//Set the wheel, tire and suspension data.
		//Set the geometry data.
		//Set the query filter data
		for (auto i = 0; i < numWheels; i++)
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

}