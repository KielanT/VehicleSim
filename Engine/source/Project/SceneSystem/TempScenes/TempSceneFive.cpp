#include "ppch.h"
#include "TempSceneFive.h"
#include "Project/EntitySystem/Components/TransformComponent.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace Project
{
	physx::PxF32 SteerVsForwardSpeedDataFive[2 * 8] =
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
	physx::PxFixedSizeLookupTable<8> SteerVsForwardSpeedTables(SteerVsForwardSpeedDataFive, 4);

	physx::PxVehicleDrive4WRawInputData VehicleInputData;

	TempSceneFive::TempSceneFive(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour /*= CVector3(1.0f, 1.0f, 1.0f)*/, float specularPower /*= 256.0f*/, ColourRGBA backgroundColour /*= ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f)*/, bool vsyncOn /*= true*/)
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

	TempSceneFive::TempSceneFive(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour /*= CVector3(1.0f, 1.0f, 1.0f)*/, float specularPower /*= 256.0f*/, ColourRGBA backgroundColour /*= ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f)*/, bool vsyncOn /*= true*/)
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

	bool TempSceneFive::InitGeometry()
	{
		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = new Camera();

		std::string path = "media/";
		m_EntityManager->CreateModelEntity("Floor", path + "Ground.x");
		m_EntityManager->CreateModelEntity("Test Cube", path + "Cube.x", true, path + "brick1.jpg");
		m_EntityManager->CreateModelEntity("Car", path + "Compact/untitled1Parented.obj", true, path + "Compact/CompactBlue.png");
		//m_EntityManager->CreateModelEntity("CarCol", path + "Compact/untitled3.obj", false, path + "Compact/CompactBlue.png"); // Doesn't move due to the origin be perfectly centred for wheels
		m_EntityManager->CreateModelEntity("CarCol", path + "Compact/untitled4.obj", false, path + "Compact/CompactBlue.png"); // Moves but is very bumpy and wheels rotation off due to orgin not being centred
		
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

			m_Material = m_PhysicsSystem->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);

			frictionPairs = CreateFrictionPairs(m_Material);

			m_VehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, m_Allocator);
			m_BatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *m_VehicleSceneQueryData, m_PhysicsSystem->GetScene());

			// Create a basic vehicle
			VehicleDesc vehicleDesc = InitVehicleDesc();
			m_Vehicle4W = CreateVehicle4W(vehicleDesc);
			m_Vehicle4W->getRigidDynamicActor()->setGlobalPose({ 0.0f, 5.0f, 0.0f });

			m_Vehicle4W->setToRestState();
			m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
			m_Vehicle4W->mDriveDynData.setUseAutoGears(true);

			VehicleInputData.setDigitalBrake(true);

			// Set Actors and shapes here
			m_BoxActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform({ 20.0f, 20.0f, 0.0f }));
			m_BoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor, physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *m_Material);
			m_BoxActor->setActorFlags(physx::PxActorFlag::eDISABLE_GRAVITY);

			//m_FloorActor = m_PhysicsSystem->GetPhysics()->createRigidStatic({ 0.0f, 0.0f, 0.0f });
			//m_FloorBoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_FloorActor, physx::PxBoxGeometry(1000.0f, 0.01f, 1000.0f), *m_Material);
			physx::PxFilterData GroundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
			m_FloorPlane = CreateDrivablePlane(GroundPlaneSimFilterData, m_Material, m_PhysicsSystem->GetPhysics());


			m_PhysicsSystem->GetScene()->addActor(*m_BoxActor);
			//m_PhysicsSystem->GetScene()->addActor(*m_FloorActor);
			m_PhysicsSystem->GetScene()->addActor(*m_FloorPlane);
			m_PhysicsSystem->GetScene()->addActor(*m_Vehicle4W->getRigidDynamicActor());
		}

		return true;
	}

	bool TempSceneFive::InitScene()
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
			float w = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.w;
			vectRot.x = m_BoxActor->getGlobalPose().q.x;
			vectRot.y = m_BoxActor->getGlobalPose().q.y;
			vectRot.z = m_BoxActor->getGlobalPose().q.z;
			comp->SetRotationFromQuat(vectRot, w);
		}

		if (m_EntityManager->GetEntity("Car")->GetComponent("Transform"))
		{

			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Car")->GetComponent("Transform"));
			CVector3 vect;
			vect.x = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().p.x;
			vect.y = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().p.y;
			vect.z = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().p.z;
			comp->SetPosition(vect);
			CVector3 vectRot;
			float w = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.w;
			vectRot.x = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.x;
			vectRot.y = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.y;
			vectRot.z = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.z;
			
			comp->SetRotationFromQuat(vectRot, w);

		}

		// Pos 1
		m_SceneCamera->SetPosition({ 0, 10, -40 });
		m_SceneCamera->SetRotation({ 0, 0, 0 });


		// Pos 2
		//m_SceneCamera->SetPosition({ -40, 10, 0 });
		//m_SceneCamera->SetRotation({ 0, ToRadians(90), 0 });

		return true;
	}

	void TempSceneFive::RenderScene()
	{
		Gui();
		m_EntityManager->RenderAllEntities();


	}

	void TempSceneFive::UpdateScene(float frameTime)
	{
		if (m_EnablePhysics)
		{
			if (m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"))
			{

				TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"));
				CVector3 vect;
				vect.x = m_BoxActor->getGlobalPose().p.x;
				vect.y = m_BoxActor->getGlobalPose().p.y;
				vect.z = m_BoxActor->getGlobalPose().p.z;
				comp->SetPosition(vect);
				CVector3 vectRot;
				float w = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.w;
				vectRot.x = m_BoxActor->getGlobalPose().q.x;
				vectRot.y = m_BoxActor->getGlobalPose().q.y;
				vectRot.z = m_BoxActor->getGlobalPose().q.z;
				comp->SetRotationFromQuat(vectRot, w);


			}

			if (m_EntityManager->GetEntity("Car")->GetComponent("Transform"))
			{

				TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Car")->GetComponent("Transform"));
				CVector3 vect;
				vect.x = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().p.x;
				vect.y = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().p.y;
				vect.z = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().p.z;
				comp->SetPosition(vect);
				CVector3 vectRot;
				float w = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.w;
				vectRot.x = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.x;
				vectRot.y = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.y;
				vectRot.z = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.z;
				comp->SetRotationFromQuat(vectRot, w);
				
			}
		}
		

		if (KeyHit(Key_F1))
		{
			m_IsDevMode = !m_IsDevMode;
		}

		if(!m_IsDevMode)
			MoveVehicle(frameTime);

		
		CameraControl(frameTime);

		

		m_EntityManager->UpdateAllEntities(frameTime);
		m_PhysicsSystem->GetScene()->simulate(frameTime);
		m_PhysicsSystem->GetScene()->fetchResults(true);
		
	}

	void TempSceneFive::ReleaseResources()
	{
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)    m_EntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);

		if (m_EnablePhysics && m_PhysicsSystem != nullptr) physx::PxCloseVehicleSDK();

		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

	void TempSceneFive::Gui()
	{
		//ImGui::ShowDemoWindow();
		
		ImGui::Begin("HUD");
		if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eREVERSE)
		{
			ImGui::Text("Gear: Reverse");
		}
		else
		{
			ImGui::Text("Gear: Drive");
		}

		/*if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eNEUTRAL)
		{
			ImGui::Text("Gear: Neutral");
		}		
		else if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eREVERSE)
		{
			ImGui::Text("Gear: Reverse");
		}
		else if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eFIRST)
		{
			ImGui::Text("Gear: First");
		}	
		else if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eSECOND)
		{
			ImGui::Text("Gear: Second");
		}
		else if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eTHIRD)
		{
			ImGui::Text("Gear: Third");
		}
		else if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eFOURTH)
		{
			ImGui::Text("Gear: Fouth");
		}
		else if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eFIFTH)
		{
			ImGui::Text("Gear: Fifth");
		}
		else if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eSIXTH)
		{
			ImGui::Text("Gear: Sixth");
		}
		else
		{
			ImGui::Text("Gear: Other");
		}*/
		
		ImGui::End();

		
		if (m_IsDevMode)
		{
			ImGui::Begin("Dev");
			ImGui::Text("Dev: On");
			ImGui::End();
		}
		
		
	}

	void TempSceneFive::CameraControl(float frameTime)
	{
		if (!m_IsDevMode && m_EntityManager->GetEntity("Car")->GetComponent("Transform"))
		{

			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Car")->GetComponent("Transform"));
			CVector3 pos = comp->GetPosition();
			pos.x -= 0.0f;
			pos.y += 5.0f;
			pos.z -= 20.0f;
			//m_SceneCamera->SetPosition(pos);

			CVector3 facingVector = comp->GetFacingVector();
			facingVector.Normalise();
			m_SceneCamera->SetPosition(comp->GetPosition() - facingVector * 15.0f + comp->GetYAxis() * 3.0f);
			
			m_SceneCamera->SetPosition({ m_SceneCamera->Position().x, m_SceneCamera->Position().y + 2, m_SceneCamera->Position().z });
	
			m_SceneCamera->FaceTarget(comp->GetPosition());

			
		}
		else
		{
			m_SceneCamera->Control(frameTime);
		}
	}

	TempSceneFive::VehicleDesc TempSceneFive::InitVehicleDesc()
	{
		const physx::PxF32 chassisMass = 1500.0f;

		const physx::PxF32 wheelMass = 20.0f;

		const physx::PxU32 nbWheels = 4;

		VehicleDesc vehicleDesc;

		vehicleDesc.chassisMass = chassisMass;
		vehicleDesc.chassisMaterial = m_Material;
		vehicleDesc.chassisSimFilterData = physx::PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

		vehicleDesc.wheelMass = wheelMass;

		vehicleDesc.numWheels = nbWheels;
		vehicleDesc.wheelMaterial = m_Material;
		vehicleDesc.chassisSimFilterData = physx::PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

		return vehicleDesc;
	}

	physx::PxVehicleDrive4W* TempSceneFive::CreateVehicle4W(const VehicleDesc& vehicle4WDesc)
	{
		physx::PxVec3 chassisDims = { 0.0f, 0.0f, 0.0f };
		physx::PxVec3 chassisCMOffset = { 0.0f, 0.0f, 0.0f };


		const physx::PxU32 numWheels = vehicle4WDesc.numWheels;
		physx::PxF32 wheelWidths[4];
		physx::PxF32 wheelRadii[4];



		const physx::PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
		const physx::PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

		//Construct a physx actor with shapes for the chassis and wheels.
		//Set the rigid body mass, moment of inertia, and center of mass offset.
		physx::PxRigidDynamic* veh4WActor = NULL;
		{
			//Construct a convex mesh for a cylindrical wheel.
			physx::PxConvexMesh* wheelMesh[4];
			/*for (int i = 0; i < 4; ++i)
			{
				wheelMesh[i] = CreateWheelMesh(i);
			}*/

			// For the compact vehicles (untitled files)
			wheelMesh[0] = CreateWheelMesh(0); // Front Left
			wheelMesh[1] = CreateWheelMesh(4); // Front right
			wheelMesh[2] = CreateWheelMesh(2); // Rear left
			wheelMesh[3] = CreateWheelMesh(3); // Rear 

			// For the compact box vehicle collision 
			//wheelMesh[0] = CreateWheelMesh(1); // Front Left
			//wheelMesh[1] = CreateWheelMesh(2); // Front right
			//wheelMesh[2] = CreateWheelMesh(3); // Rear left
			//wheelMesh[3] = CreateWheelMesh(4); // Rear right

			MakeWheelWidthsAndRadii(wheelMesh, wheelWidths, wheelRadii);

			//Assume all wheels are identical for simplicity.
			physx::PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
			physx::PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

			//Set the meshes and materials for the driven wheels.
			for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
			{
				wheelConvexMeshes[i] = wheelMesh[i];
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}

			//Chassis just has a single convex shape for simplicity.
			physx::PxConvexMesh* chassisConvexMesh = CreateChassisMesh(1); // Default chassis for the untitled files
			//physx::PxConvexMesh* chassisConvexMesh = CreateChassisMesh(0); // Default chassis for the collision box
			physx::PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
			physx::PxMaterial* chassisMaterials[1] = { vehicle4WDesc.chassisMaterial };

			chassisDims = MakeChassis(chassisConvexMeshes[0]);

			physx::PxVec3 chassisMOI
			((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * vehicle4WDesc.chassisMass / 12.0f,
				(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * vehicle4WDesc.chassisMass / 12.0f,
				(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * vehicle4WDesc.chassisMass / 12.0f);

			chassisCMOffset = { 0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f }; // Sets the centre mass

			//Rigid body data.
			physx::PxVehicleChassisData rigidBodyData;
			rigidBodyData.mMOI = chassisMOI;
			rigidBodyData.mMass = vehicle4WDesc.chassisMass;
			rigidBodyData.mCMOffset = chassisCMOffset;

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
			ComputeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidths, wheelRadii, numWheels, wheelCenterActorOffsets);

			physx::PxF32 wheelMOI[4];

			for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
			{
				wheelMOI[i] = 0.5f * vehicle4WDesc.wheelMass * wheelRadii[i] * wheelRadii[i];
			}

			//Set up the simulation data for all wheels.
			SetupWheelsSimulationData
			(vehicle4WDesc.wheelMass, wheelMOI, wheelRadii, wheelWidths,
				numWheels, wheelCenterActorOffsets,
				chassisCMOffset, vehicle4WDesc.chassisMass,
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
			gears.mSwitchTime = 0.1f;
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

		//Free the sim data because we don't need that any more.
		wheelsSimData->free();

		return vehDrive4W;
	}

	physx::PxConvexMesh* TempSceneFive::CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts)
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

	physx::PxConvexMesh* TempSceneFive::CreateWheelMesh(int index)
	{
		physx::PxU32 vertexCount;
		std::vector<physx::PxVec3> vertices;
		//if (m_EntityManager->GetEntity("Car")->GetComponent("Renderer"))
		if (m_EntityManager->GetEntity("CarCol")->GetComponent("Renderer"))
		{
			//RendererComponent* comp = static_cast<RendererComponent*>(m_EntityManager->GetEntity("Car")->GetComponent("Renderer"));
			RendererComponent* comp = static_cast<RendererComponent*>(m_EntityManager->GetEntity("CarCol")->GetComponent("Renderer"));
			vertexCount = comp->GetNumberOfVertices(index);

			std::vector<CVector3> Wheels = comp->GetVertices();

			for (int i = 0; i < vertexCount; ++i)
			{
				vertices.push_back({ Wheels[i].x,  Wheels[i].y, Wheels[i].z });
			}
		}

		physx::PxVec3* v = vertices.data();

		return CreateConvexMesh(v, vertexCount);
	}

	physx::PxConvexMesh* TempSceneFive::CreateChassisMesh(int index)
	{
		physx::PxU32 vertexCount;
		std::vector<physx::PxVec3> vertices;
		if (m_EntityManager->GetEntity("Car")->GetComponent("Renderer"))
			//if (m_EntityManager->GetEntity("CarCol")->GetComponent("Renderer"))
		{
			RendererComponent* comp = static_cast<RendererComponent*>(m_EntityManager->GetEntity("CarCol")->GetComponent("Renderer"));
			//RendererComponent* comp = static_cast<RendererComponent*>(m_EntityManager->GetEntity("Car")->GetComponent("Renderer"));
			vertexCount = comp->GetNumberOfVertices(index);

			std::vector<CVector3> Chassis = comp->GetVertices();

			for (int i = 0; i < vertexCount; ++i)
			{
				vertices.push_back({ Chassis[i].x,  Chassis[i].y, Chassis[i].z });
			}
		}

		physx::PxVec3* v = vertices.data();

		return CreateConvexMesh(v, vertexCount);
	}

	physx::PxRigidDynamic* TempSceneFive::CreateVehicleActor(const physx::PxVehicleChassisData& chassisData, physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, const physx::PxFilterData& wheelSimFilterData, physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData)
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

	void TempSceneFive::ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32* wheelWidth, const physx::PxF32* wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets)
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
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] =   physx::PxVec3((-chassisDims.x + wheelWidth[2]) * 0.5f, -(chassisDims.y / 2 + wheelRadius[2]),  wheelRearZ + 0 * deltaZ * 0.5f);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] =  physx::PxVec3((+chassisDims.x - wheelWidth[3]) * 0.5f, -(chassisDims.y / 2 + wheelRadius[3]),  wheelRearZ + 0 * deltaZ * 0.5f);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] =  physx::PxVec3((-chassisDims.x + wheelWidth[0]) * 0.5f, -(chassisDims.y / 2 + wheelRadius[0]),  wheelRearZ + (numLeftWheels - 1) * deltaZ);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth[1]) * 0.5f, -(chassisDims.y / 2 + wheelRadius[1]),  wheelRearZ + (numLeftWheels - 1) * deltaZ);

		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth[2]) * 0.5f,   -0.3f, wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth[3]) * 0.5f,  -0.3f, wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth[0]) * 0.5f,  -0.3f, wheelRearZ + (numLeftWheels - 1) * deltaZ);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth[1]) * 0.5f, -0.3f, wheelRearZ + (numLeftWheels - 1) * deltaZ);


		// Hardcoded to work (possible needs different values per vehicle) for untitled1
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] =   physx::PxVec3(-0.0f, -0.3f, -2.0f);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] =  physx::PxVec3( 1.2f, -0.3f, -2.0f);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] =  physx::PxVec3(-0.0f, -0.3f,  0.0f);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3( 1.2f, -0.3f,  0.0f);

		// Hardcoded to work (possible needs different values per vehicle) for untitled1parented
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT]   = physx::PxVec3(-0.0f, -0.3f, -2.0f);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT]  = physx::PxVec3( 1.2f, -0.3f, -2.0f);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT]  = physx::PxVec3(-0.0f, -0.3f,  0.0f);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3( 1.2f, -0.3f,  0.0f);

		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] =   physx::PxVec3(-chassisDims.x + wheelWidth[2], chassisDims.y - 1.8f, wheelRearZ + 0 * deltaZ);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] =  physx::PxVec3(+chassisDims.x - wheelWidth[3], chassisDims.y - 1.8f,   wheelRearZ + 0 * deltaZ);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] =  physx::PxVec3(-chassisDims.x + wheelWidth[0], chassisDims.y - 1.8f,  wheelRearZ + (numLeftWheels - 1) * deltaZ);
		//wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3(+chassisDims.x - wheelWidth[1], chassisDims.y - 1.8f,  wheelRearZ + (numLeftWheels - 1) * deltaZ); 
	}

	void TempSceneFive::SetupWheelsSimulationData(const physx::PxF32 wheelMass, const physx::PxF32* wheelMOI, const physx::PxF32* wheelRadius, const physx::PxF32* wheelWidth, const physx::PxU32 numWheels, const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset, const physx::PxF32 chassisMass, physx::PxVehicleWheelsSimData* wheelsSimData)
	{
		//Set up the wheels.
		physx::PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
		{
			//Set up the wheel data structures with mass, moi, radius, width.
			for (auto i = 0; i < numWheels; i++)
			{
				wheels[i].mMass = wheelMass;
				wheels[i].mMOI = wheelMOI[i];
				wheels[i].mRadius = wheelRadius[i];
				wheels[i].mWidth = wheelWidth[i];
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
		wheelsSimData->setSubStepCount(5.0f, 3, 1);

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

	physx::PxVec3 TempSceneFive::MakeChassis(physx::PxConvexMesh* chassisConvexMesh)
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

	void TempSceneFive::MakeWheelWidthsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii)
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
			//wheelRadii[i] = physx::PxMax(wheelMax.x, wheelMax.y) * 0.975f;
			//wheelRadii[i] = 200;// physx::PxMax(wheelMax.y, wheelMax.z) * 0.975f;
			//wheelRadii[i] = 20;// physx::PxMax(wheelMax.y, wheelMax.z) * 0.975f;

		}
	}
	static physx::PxF32 gTireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
	{
		//NORMAL,	WORN
		{1.00f,		0.1f}//TARMAC
	};
	
	
	void TempSceneFive::MoveVehicle(float frameTime)
	{
		Controls();


		if (KeyHeld(Key_W))
			m_Accelerate = true;

		if (KeyHeld(Key_D))
			m_Right = true;
		
		if (KeyHeld(Key_A))
			m_Left = true;
		
		if (KeyHeld(Key_S))
			m_Brake = true;
		
		if (KeyHeld(Key_Q))
			m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
		
		if (KeyHeld(Key_E))
			m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);

		
		physx::PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(keySmoothingData, SteerVsForwardSpeedTables, VehicleInputData, frameTime, IsVehicleInAir, *m_Vehicle4W);

		//Raycasts.
		physx::PxVehicleWheels* vehicles[1] = { m_Vehicle4W };
		physx::PxRaycastQueryResult* raycastResults = m_VehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const physx::PxU32 raycastResultsSize = m_VehicleSceneQueryData->getQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(m_BatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

		//Vehicle update.
		const physx::PxVec3 grav = m_PhysicsSystem->GetScene()->getGravity();
		physx::PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		physx::PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, m_Vehicle4W->mWheelsSimData.getNbWheels()} };
		PxVehicleUpdates(frameTime, grav, *frictionPairs, 1, vehicles, vehicleQueryResults);

		//Work out if the vehicle is in the air.
		IsVehicleInAir = m_Vehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
	}

	void TempSceneFive::Controls()
	{
		VehicleInputData.setDigitalAccel(m_Accelerate);
		VehicleInputData.setDigitalSteerLeft(m_Left);
		VehicleInputData.setDigitalSteerRight(m_Right);
		VehicleInputData.setDigitalBrake(m_Brake);
		VehicleInputData.setDigitalHandbrake(m_HandBrake);

		m_Accelerate = false;
		m_Left = false;
		m_Right = false;
		m_Brake = false;
		m_HandBrake = false;

				
	}

	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* TempSceneFive::CreateFrictionPairs(const physx::PxMaterial* defaultMaterial)
	{
		physx::PxVehicleDrivableSurfaceType surfaceTypes[1];
		surfaceTypes[0].mType = SURFACE_TYPE_TARMAC;

		const physx::PxMaterial* surfaceMaterials[1];
		surfaceMaterials[0] = defaultMaterial;

		physx::PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
			physx::PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES);

		surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES, surfaceMaterials, surfaceTypes);

		for (physx::PxU32 i = 0; i < MAX_NUM_SURFACE_TYPES; i++)
		{
			for (physx::PxU32 j = 0; j < MAX_NUM_TIRE_TYPES; j++)
			{
				surfaceTirePairs->setTypePairFriction(i, j, gTireFrictionMultipliers[i][j]);
			}
		}
		return surfaceTirePairs;
	}

	physx::PxRigidStatic* TempSceneFive::CreateDrivablePlane(const physx::PxFilterData& simFilterData, physx::PxMaterial* material, physx::PxPhysics* physics)
	{
		//Add a plane to the scene.
		physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*physics, physx::PxPlane(0, 1, 0, 0), *material);

		//Get the plane shape so we can set query and simulation filter data.
		physx::PxShape* shapes[1];
		groundPlane->getShapes(shapes, 1);

		//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
		physx::PxFilterData qryFilterData;
		setupDrivableSurface(qryFilterData);
		shapes[0]->setQueryFilterData(qryFilterData);

		//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
		shapes[0]->setSimulationFilterData(simFilterData);

		return groundPlane;
	}

	/*void TempSceneFive::SetAutoBoxData(const physx::PxVehicleAutoBoxData& autobox)
	{
		PX_CHECK_AND_RETURN(autobox.mUpRatios  [physx::PxVehicleGearsData::eREVERSE] >= 0, "Autobox gearup ratio in reverse must be greater than or equal to zero");
		PX_CHECK_AND_RETURN(autobox.mUpRatios  [physx::PxVehicleGearsData::eNEUTRAL] >= 0, "Autobox gearup ratio in neutral must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mUpRatios  [physx::PxVehicleGearsData::eFIRST] >= 0, "Autobox gearup ratio in first must be greater than or equal to zero");
		PX_CHECK_AND_RETURN(autobox.mUpRatios  [physx::PxVehicleGearsData::eSECOND] >= 0, "Autobox gearup ratio in second must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mUpRatios  [physx::PxVehicleGearsData::eTHIRD] >= 0, "Autobox gearup ratio in third must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mUpRatios  [physx::PxVehicleGearsData::eFOURTH] >= 0, "Autobox gearup ratio in fourth must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mUpRatios  [physx::PxVehicleGearsData::eFIFTH] >= 0, "Autobox gearup ratio in fifth must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mDownRatios[physx::PxVehicleGearsData::eREVERSE] >= 0, "Autobox geardown ratio in reverse must be greater than or equal to zero");
		PX_CHECK_AND_RETURN(autobox.mDownRatios[physx::PxVehicleGearsData::eNEUTRAL] >= 0, "Autobox geardown ratio in neutral must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mDownRatios[physx::PxVehicleGearsData::eFIRST] >= 0, "Autobox geardown ratio in first must be greater than or equal to zero");
		PX_CHECK_AND_RETURN(autobox.mDownRatios[physx::PxVehicleGearsData::eSECOND] >= 0, "Autobox geardown ratio in second must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mDownRatios[physx::PxVehicleGearsData::eTHIRD] >= 0, "Autobox geardown ratio in third must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mDownRatios[physx::PxVehicleGearsData::eFOURTH] >= 0, "Autobox geardown ratio in fourth must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mDownRatios[physx::PxVehicleGearsData::eFIFTH] >= 0, "Autobox geardown ratio in fifth must be greater than zero");
		PX_CHECK_AND_RETURN(autobox.mDownRatios[physx::PxVehicleGearsData::eSIXTH] >= 0, "Autobox geardown ratio in fifth must be greater than zero");
		m_AutoBox = autobox;
	}*/

}
