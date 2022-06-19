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


        // Create the vehicles

        m_Vehicle = CreateVehicle();
        m_Vehicle->getRigidDynamicActor()->setGlobalPose({ m_Transform->GetPosition().x, m_Transform->GetPosition().y, m_Transform->GetPosition().z });

       // m_Vehicle->setToRestState();
       // m_Vehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
       // m_Vehicle->mDriveDynData.setUseAutoGears(true);

        //VehicleInputData.setDigitalBrake(true);
       m_Physics->GetScene()->addActor(*m_Vehicle->getRigidDynamicActor());

    }

    physx::PxVehicleDrive4W* VehicleComponent::CreateVehicle()
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
			(m_VehicleSettings, wheelMOI, wheelRadii, wheelWidths, wheelCenterActorOffsets,
				chassisCMOffset, wheelsSimData);
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

		return CreateConvexMesh(m_Physics, v, vertexCount);
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

		return CreateConvexMesh(m_Physics, v, vertexCount);
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

	void VehicleComponent::ComputeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, const physx::PxF32* wheelWidth, const physx::PxF32* wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets)
	{

		const physx::PxF32 numLeftWheels = numWheels / 2.0f;
		const physx::PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f);
		
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth[2]) * 0.5f, -0.3f, wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth[3]) * 0.5f, -0.3f, wheelRearZ + 0 * deltaZ * 0.5f);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth[0]) * 0.5f, -0.3f, wheelRearZ + (numLeftWheels - 1) * deltaZ);
		wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth[1]) * 0.5f, -0.3f, wheelRearZ + (numLeftWheels - 1) * deltaZ);
	}


   
}
