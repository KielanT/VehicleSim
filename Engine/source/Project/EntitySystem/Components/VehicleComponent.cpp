#include "ppch.h"
#include "VehicleComponent.h"
#include "CollisionComponent.h"

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

            // Move Vehicle

            return true;
        }
        else
        {
            return false;
        }
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
        //m_FrictionPairs = CreateFrictionPairs();

        // Create vehicle query for movement
        //m_VehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, m_Allocator);
        //m_BatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *m_VehicleSceneQueryData, m_Physics->GetScene());

        // TODO REMOVE THIS
        physx::PxRigidDynamic* boxActor = m_Physics->GetPhysics()->createRigidDynamic(physx::PxTransform({ 20.0f, 10.0f, 0.0f }));
        physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*boxActor, physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *m_Material);
        boxActor->setActorFlags(physx::PxActorFlag::eDISABLE_GRAVITY);

        // Create the vehicles

		
        m_Vehicle = CreateVehicle4W();
        //m_Vehicle->getRigidDynamicActor()->setGlobalPose({ m_Transform->GetPosition().x, m_Transform->GetPosition().y, m_Transform->GetPosition().z });

        m_Vehicle->getRigidDynamicActor()->setGlobalPose({ 0.0f, 0.0f, 0.0f });

       // m_Vehicle->setToRestState();
       // m_Vehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
       // m_Vehicle->mDriveDynData.setUseAutoGears(true);

        //VehicleInputData.setDigitalBrake(true);
       m_Physics->GetScene()->addActor(*boxActor);
       m_Physics->GetScene()->addActor(*m_Vehicle->getRigidDynamicActor());

    }

    physx::PxVehicleDrive4W* VehicleComponent::CreateVehicle4W()
    {
		physx::PxVec3 chassisDims = { 0.0f, 0.0f, 0.0f };
		physx::PxVec3 chassisCMOffset = { 0.0f, 0.0f, 0.0f };

		physx::PxF32 wheelWidths[PX_MAX_NB_WHEELS];
		physx::PxF32 wheelRadii[PX_MAX_NB_WHEELS];


		const physx::PxFilterData& chassisSimFilterData = physx::PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);
		const physx::PxFilterData& wheelSimFilterData = physx::PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);


		//Construct a physx actor with shapes for the chassis and wheels.
		//Set the rigid body mass, moment of inertia, and center of mass offset.
		physx::PxRigidDynamic* veh4WActor = NULL;
		{
			//Construct a convex mesh for a cylindrical wheel.
			physx::PxConvexMesh* wheelMesh[PX_MAX_NB_WHEELS];

			// For the compact vehicles (untitled files)
			wheelMesh[0] = CreateWheelMesh(0); 
			wheelMesh[1] = CreateWheelMesh(4); 
			wheelMesh[2] = CreateWheelMesh(2); 
			wheelMesh[3] = CreateWheelMesh(3); 



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
			physx::PxConvexMesh* chassisConvexMesh = CreateChassisMesh(1); 
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

			physx::PxF32 wheelMOI[PX_MAX_NB_WHEELS];

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

	physx::PxConvexMesh* VehicleComponent::CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts)
	{
		// Create descriptor for convex mesh
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = numVerts;
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = verts;
		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxConvexMesh* convexMesh = NULL;
		physx::PxDefaultMemoryOutputStream buf;
		if (m_Physics->GetCooking()->cookConvexMesh(convexDesc, buf))
		{
			physx::PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
			convexMesh = m_Physics->GetPhysics()->createConvexMesh(id);
		}

		return convexMesh;
	}

	physx::PxConvexMesh* VehicleComponent::CreateWheelMesh(int index)
	{
		physx::PxU32 vertexCount;
		std::vector<physx::PxVec3> vertices;
		if (m_Entity->GetComponent("CollisionMesh"))
		{
			CollisionComponent* comp = static_cast<CollisionComponent*>(m_Entity->GetComponent("CollisionMesh"));
			vertexCount = comp->GetNumberOfVertices(index);

			std::vector<CVector3> Wheels = comp->GetVertices(index);

			for (int i = 0; i < vertexCount; ++i)
			{
				vertices.push_back({ Wheels[i].x,  Wheels[i].y, Wheels[i].z });
			}
		}

		physx::PxVec3* v = vertices.data();

		return CreateConvexMesh(v, vertexCount);
	}

	physx::PxConvexMesh* VehicleComponent::CreateChassisMesh(int index)
	{
		physx::PxU32 vertexCount;
		std::vector<physx::PxVec3> vertices;
		if (m_Entity->GetComponent("CollisionMesh"))
		{
			CollisionComponent* comp = static_cast<CollisionComponent*>(m_Entity->GetComponent("CollisionMesh"));
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

	void VehicleComponent::MakeWheelWidthsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii)
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

	physx::PxVec3 VehicleComponent::MakeChassis(physx::PxConvexMesh* chassisConvexMesh)
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

	physx::PxRigidDynamic* VehicleComponent::CreateVehicleActor(const physx::PxVehicleChassisData& chassisData, physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, const physx::PxFilterData& wheelSimFilterData, physx::PxMaterial** chassisMaterials, physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, const physx::PxFilterData& chassisSimFilterData)
	{
		//We need a rigid body actor for the vehicle.
		//Don't forget to add the actor to the scene after setting up the associated vehicle.
		physx::PxRigidDynamic* vehActor = m_Physics->GetPhysics()->createRigidDynamic(physx::PxTransform(physx::PxIdentity));

		//Wheel and chassis query filter data.
		//Optional: cars don't drive on other cars.
		physx::PxFilterData wheelQryFilterData;
		SetupNonDrivableSurface(wheelQryFilterData);
		physx::PxFilterData chassisQryFilterData;
		SetupNonDrivableSurface(chassisQryFilterData);

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

	void VehicleComponent::ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32* wheelWidth, const physx::PxF32* wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets)
	{

		const physx::PxF32 numLeftWheels = numWheels / 2.0f;
		const physx::PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f);
		
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth[2]) * 0.5f, -0.3f, wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth[3]) * 0.5f, -0.3f, wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth[0]) * 0.5f, -0.3f, wheelRearZ + (numLeftWheels - 1) * deltaZ);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth[1]) * 0.5f, -0.3f, wheelRearZ + (numLeftWheels - 1) * deltaZ);
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
		SetupNonDrivableSurface(qryFilterData);

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

		if (m_VehicleSettings.GetAntiRollBar() == AntiRollBar::Both)
		{
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
		else if (m_VehicleSettings.GetAntiRollBar() == AntiRollBar::FrontOnly)
		{
			//Add a front and rear anti-roll bar
			physx::PxVehicleAntiRollBarData barFront;
			barFront.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
			barFront.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
			barFront.mStiffness = 10000.0f;
			wheelsSimData->addAntiRollBarData(barFront);
		}
		else if (m_VehicleSettings.GetAntiRollBar() == AntiRollBar::RearOnly)
		{

			physx::PxVehicleAntiRollBarData barRear;
			barRear.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT;
			barRear.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
			barRear.mStiffness = 10000.0f;
			wheelsSimData->addAntiRollBarData(barRear);
		}
	}

    const physx::PxVehicleDifferential4WData VehicleSettings::DefaultDifferetial()
    {
        physx::PxVehicleDifferential4WData data;
        data.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
		
        return data;
    }
    const physx::PxVehicleEngineData VehicleSettings::DefaultEngine()
    {
        physx::PxVehicleEngineData data;
		data.mPeakTorque = 500.0f;
		data.mMaxOmega = 600.0f; // 6000rpm
        return data;
    }
    const physx::PxVehicleGearsData VehicleSettings::DefaultGears()
    {
        physx::PxVehicleGearsData data;
        data.mSwitchTime = 0.1f;
        return data;
    }
    const physx::PxVehicleClutchData VehicleSettings::DefaultClutch()
    {
        physx::PxVehicleClutchData data;
        data.mStrength = 10.0f;
        return data;
    }
    const physx::PxVehicleSuspensionData VehicleSettings::DefaultSuspension()
    {
        physx::PxVehicleSuspensionData data;
        data.mMaxCompression = 0.3f;
        data.mMaxDroop = 0.1f;
        data.mSpringStrength = 35000.0f;
        data.mSpringDamperRate = 4500.0f;
        return data;
    }
}
