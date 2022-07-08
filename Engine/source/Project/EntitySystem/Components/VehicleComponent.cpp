#include "ppch.h"
#include "VehicleComponent.h"
#include "CollisionComponent.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace Project
{
    VehicleComponent::~VehicleComponent()
    {
        if (m_Physics != nullptr) physx::PxCloseVehicleSDK();
    }

    bool VehicleComponent::Update(float frameTime)
    {
        if (m_Physics != nullptr)
        {
			// Update vehicle positioning and rotation
			UpdateVehiclePosAndRot();
			
            // Move Vehicle
			MoveVehicle(frameTime);

			// Move Camera
			MoveCamera();

            return true;
        }
        else
        {
            return false;
        }
    }

	void VehicleComponent::GearsUI()
	{
		ImGui::Begin("HUD");
		if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eNEUTRAL)
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
		}

		ImGui::End();
	}

	void VehicleComponent::SetupVehicle()
    {
        if (m_Entity->GetComponent("Transform"))
        {
            m_Transform = static_cast<TransformComponent*>(m_Entity->GetComponent("Transform"));
        }

        // Setup the vehicle sdk
        physx::PxInitVehicleSDK(*m_Physics->GetPhysics());
        physx::PxVehicleSetBasisVectors(physx::PxVec3(0, 1, 0), physx::PxVec3(0, 0, 1));
        physx::PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);


        // Create Material
        m_Material = m_Physics->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);

        // Create friction pairs
		 m_FrictionPairs = CreateFrictionPairs(m_Material);

        // Create vehicle query for movement
        m_VehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, m_Allocator);
        m_BatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *m_VehicleSceneQueryData, m_Physics->GetScene());

        // Create the vehicles

		m_Vehicle4W = CreateVehicle4W();
		m_Vehicle4W->getRigidDynamicActor()->setGlobalPose({ m_Transform->GetPosition().x, m_Transform->GetPosition().y, m_Transform->GetPosition().z });

		m_Vehicle4W->setToRestState();
		m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
		m_Vehicle4W->mDriveDynData.setUseAutoGears(true);

        m_VehicleInputData.setDigitalBrake(true);
		
		m_Physics->GetScene()->addActor(*m_Vehicle4W->getRigidDynamicActor());

    }


	physx::PxVehicleDrive4W* VehicleComponent::CreateVehicle4W()
	{

		physx::PxVec3 chassisDims = { 0.0f, 0.0f, 0.0f };
		physx::PxVec3 chassisCMOffset = { 0.0f, 0.0f, 0.0f };


		physx::PxF32 wheelWidths[PX_MAX_NB_WHEELS];
		physx::PxF32 wheelRadii[PX_MAX_NB_WHEELS];



		const physx::PxFilterData& chassisSimFilterData = physx::PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);
		const physx::PxFilterData& wheelSimFilterData = physx::PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

		//Construct a physx actor with shapes for the chassis and wheels.
		//Set the rigid body mass, moment of inertia, and center of mass offset.
		physx::PxRigidDynamic* veh4WActor = NULL;
		{
			//Construct a convex mesh for a cylindrical wheel.
			physx::PxConvexMesh* wheelMesh[PX_MAX_NB_WHEELS];


			// TODO Find a way to have the wheels selected in the order (remove hard coding)
			wheelMesh[0] = CreateWheelMesh(0, m_Entity, m_Physics); // Front Left
			wheelMesh[1] = CreateWheelMesh(4, m_Entity, m_Physics); // Front right
			wheelMesh[2] = CreateWheelMesh(2, m_Entity, m_Physics); // Rear left
			wheelMesh[3] = CreateWheelMesh(3, m_Entity, m_Physics); // Rear 


			MakeWheelWidthsAndRadii(wheelMesh, wheelWidths, wheelRadii);

			//Assume all wheels are identical for simplicity.
			physx::PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
			physx::PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

			//Set the meshes and materials for the driven wheels.
			for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
			{
				wheelConvexMeshes[i] = wheelMesh[i];
				wheelMaterials[i] = m_Material;
			}

			//Chassis just has a single convex shape for simplicity.
			physx::PxConvexMesh* chassisConvexMesh = CreateChassisMesh(1, m_Entity, m_Physics); 
			physx::PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
			physx::PxMaterial* chassisMaterials[1] = { m_Material };

			chassisDims = MakeChassis(chassisConvexMeshes[0]);

			physx::PxVec3 chassisMOI
			((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * m_VehicleSettings.GetChassisMass() / 12.0f,
				(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * m_VehicleSettings.GetChassisMass() / 12.0f,
				(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * m_VehicleSettings.GetChassisMass() / 12.0f);

			chassisCMOffset = { 0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f }; // Sets the centre mass

			//Rigid body data.
			physx::PxVehicleChassisData rigidBodyData;
			rigidBodyData.mMOI = chassisMOI;
			rigidBodyData.mMass = m_VehicleSettings.GetChassisMass();
			rigidBodyData.mCMOffset = chassisCMOffset;

			veh4WActor = CreateVehicleActor
			(rigidBodyData,
				wheelMaterials, wheelConvexMeshes, m_VehicleSettings.GetNumberOfWheels(), wheelSimFilterData,
				chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData);
		}

		//Set up the sim data for the wheels.
		physx::PxVehicleWheelsSimData* wheelsSimData = physx::PxVehicleWheelsSimData::allocate(m_VehicleSettings.GetNumberOfWheels());
		{
			//Compute the wheel center offsets from the origin.
			physx::PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
			const physx::PxF32 frontZ = chassisDims.z * 0.3f;
			const physx::PxF32 rearZ = -chassisDims.z * 0.3f;
			ComputeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidths, wheelRadii, m_VehicleSettings.GetNumberOfWheels(), wheelCenterActorOffsets);

			physx::PxF32 wheelMOI[4];

			for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
			{
				wheelMOI[i] = 0.5f * m_VehicleSettings.GetWheelMass() * wheelRadii[i] * wheelRadii[i];
			}

			//Set up the simulation data for all wheels.
			SetupWheelsSimulationData
			(m_VehicleSettings.GetWheelMass(), wheelMOI, wheelRadii, wheelWidths,
				m_VehicleSettings.GetNumberOfWheels(), wheelCenterActorOffsets,
				chassisCMOffset, m_VehicleSettings.GetChassisMass(),
				wheelsSimData);
		}

		//Set up the sim data for the vehicle drive model.
		physx::PxVehicleDriveSimData4W driveSimData;
		{
			//Diff
			driveSimData.setDiffData(m_VehicleSettings.GetDiff());

			//Engine
			driveSimData.setEngineData(m_VehicleSettings.GetEngine());

			//Gears
			driveSimData.setGearsData(m_VehicleSettings.GetGears());

			//Clutch
			driveSimData.setClutchData(m_VehicleSettings.GetClutch());



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
		physx::PxVehicleDrive4W* vehDrive4W = physx::PxVehicleDrive4W::allocate(m_VehicleSettings.GetNumberOfWheels());
		vehDrive4W->setup(m_Physics->GetPhysics(), veh4WActor, *wheelsSimData, driveSimData, m_VehicleSettings.GetNumberOfWheels() - 4);

		//Free the sim data because we don't need that any more.
		wheelsSimData->free();

		return vehDrive4W;
		
	}

	physx::PxRigidDynamic* VehicleComponent::CreateVehicleActor(const physx::PxVehicleChassisData& chassisData, physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, const physx::PxFilterData& wheelSimFilterData, physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData)
	{
		//We need a rigid body actor for the vehicle.
	//Don't forget to add the actor to the scene after setting up the associated vehicle.
		physx::PxRigidDynamic* vehActor = m_Physics->GetPhysics()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));

		//Wheel and chassis query filter data.
		//Optional: cars don't drive on other cars.
		physx::PxFilterData wheelQryFilterData;
		NonDrivableSurface(wheelQryFilterData);
		physx::PxFilterData chassisQryFilterData;
		NonDrivableSurface(chassisQryFilterData);

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

	void VehicleComponent::SetupWheelsSimulationData(const physx::PxF32 wheelMass, const physx::PxF32* wheelMOI, const physx::PxF32* wheelRadius, const physx::PxF32* wheelWidth, const physx::PxU32 numWheels, const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset, const physx::PxF32 chassisMass, physx::PxVehicleWheelsSimData* wheelsSimData)
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

			if (m_VehicleSettings.GetHandBrake() == HandBrake::RearWheelsOnly)
			{
				//Enable the handbrake for the rear wheels only.
				wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = m_VehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = m_VehicleSettings.GetHandBrakeTorque();
			}
			else if (m_VehicleSettings.GetHandBrake() == HandBrake::FrontWheelsOnly)
			{
				//Enable the handbrake for the Front wheels only.
				wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxHandBrakeTorque = m_VehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxHandBrakeTorque = m_VehicleSettings.GetHandBrakeTorque();
			}
			else if (m_VehicleSettings.GetHandBrake() == HandBrake::RearWheelsOnly)
			{
				//Enable the handbrake for the all wheels.
				wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = m_VehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = m_VehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxHandBrakeTorque = m_VehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxHandBrakeTorque = m_VehicleSettings.GetHandBrakeTorque();
			}
			//Enable steering for the front wheels only. // TODO Set up other drive settings
			wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = m_VehicleSettings.GetMaxSteer();
			wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = m_VehicleSettings.GetMaxSteer();
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
				suspensions[i] = m_VehicleSettings.GetSuspension(i);
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
		NonDrivableSurface(qryFilterData);

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





	void VehicleComponent::MoveVehicle(float frameTime)
	{
		UpdateInput();

		if (KeyHeld(m_Controls.accelerate))
			m_Accelerate = true;

		if (KeyHeld(m_Controls.steerRight))
			m_Right = true;

		if (KeyHeld(m_Controls.steerLeft))
			m_Left = true;

		if (KeyHeld(m_Controls.brake))
			m_Brake = true;

		if (KeyHeld(m_Controls.gearUp))
			m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);

		if (KeyHeld(m_Controls.gearDown))
			m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);
		
		physx::PxFixedSizeLookupTable<8> SteerVsForwardSpeedTablesComp(SteerVsForwardSpeedDataComp, 4);
		
		physx::PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(KeySmoothingData, SteerVsForwardSpeedTablesComp, m_VehicleInputData, frameTime, IsVehicleInAir, *m_Vehicle4W);

		//Raycasts.
		physx::PxVehicleWheels* vehicles[1] = { m_Vehicle4W };
		physx::PxRaycastQueryResult* raycastResults = m_VehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const physx::PxU32 raycastResultsSize = m_VehicleSceneQueryData->getQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(m_BatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

		//Vehicle update.
		const physx::PxVec3 grav = m_Physics->GetScene()->getGravity();
		physx::PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		physx::PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, m_Vehicle4W->mWheelsSimData.getNbWheels()} };
		PxVehicleUpdates(frameTime, grav, *m_FrictionPairs, 1, vehicles, vehicleQueryResults);

		//Work out if the vehicle is in the air.
		IsVehicleInAir = m_Vehicle4W->getRigidDynamicActor()->isSleeping() ? false : physx::PxVehicleIsInAir(vehicleQueryResults[0]);
	}

	void VehicleComponent::UpdateVehiclePosAndRot()
	{
		CVector3 pos;
		pos.x = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().p.x;
		pos.y = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().p.y - 0.6f;
		pos.z = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().p.z;
		m_Transform->SetPosition(pos);
		CVector3 rot;
		float w = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.w;
		rot.x = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.x;
		rot.y = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.y;
		rot.z = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose().q.z;
		m_Transform->SetRotationFromQuat(rot, w);


		/*physx::PxShape* shapeBuffer[4];
		m_Vehicle4W->getRigidDynamicActor()->getShapes(shapeBuffer, m_Vehicle4W->mWheelsSimData.getNbWheels());
		const physx::PxTransform vehGlobalPose = m_Vehicle4W->getRigidDynamicActor()->getGlobalPose();
		const physx::PxTransform wheelTransform = vehGlobalPose.transform(shapeBuffer[1]->getLocalPose());
		float wRot = wheelTransform.q.w;
		CVector3 rotation = CVector3(wheelTransform.q.x, wheelTransform.q.y, wheelTransform.q.z);
		m_Transform->SetRotationFromQuat(rotation, w, 3);*/
	}

	void VehicleComponent::UpdateInput()
	{
		m_VehicleInputData.setDigitalAccel(m_Accelerate);
		m_VehicleInputData.setDigitalSteerLeft(m_Left);
		m_VehicleInputData.setDigitalSteerRight(m_Right);
		m_VehicleInputData.setDigitalBrake(m_Brake);
		m_VehicleInputData.setDigitalHandbrake(m_HandBrake);

		m_Accelerate = false;
		m_Left = false;
		m_Right = false;
		m_Brake = false;
		m_HandBrake = false;

	}

	void VehicleComponent::MoveCamera()
	{
		if (m_Camera != nullptr)
		{
			CVector3 facingVector = m_Transform->GetFacingVector();
			facingVector.Normalise();
			m_Camera->SetPosition(m_Transform->GetPosition() - facingVector * 15.0f + m_Transform->GetYAxis() * 3.0f);

			m_Camera->SetPosition({ m_Camera->Position().x, m_Camera->Position().y + 2, m_Camera->Position().z });

			m_Camera->FaceTarget(m_Transform->GetPosition());
		}
	}


   
}
