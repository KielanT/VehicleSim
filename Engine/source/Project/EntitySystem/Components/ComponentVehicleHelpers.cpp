#include "ppch.h"
#include "ComponentVehicleHelpers.h"


namespace Project
{
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
    void SetupWheelsSimulationData(VehicleSettings vehicleSettings, const physx::PxF32* wheelMOI, const physx::PxF32* wheelRadius, const physx::PxF32* wheelWidth, const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset, physx::PxVehicleWheelsSimData* wheelsSimData)
    {
		//Set up the wheels.
		physx::PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
		{
			//Set up the wheel data structures with mass, moi, radius, width.
			for (auto i = 0; i < vehicleSettings.GetNumberOfWheels(); i++)
			{
				wheels[i].mMass = vehicleSettings.GetWheelMass();
				wheels[i].mMOI = wheelMOI[i];
				wheels[i].mRadius = wheelRadius[i];
				wheels[i].mWidth = wheelWidth[i];
			}
			if (vehicleSettings.GetHandBrake() == HandBrake::RearWheelsOnly)
			{
				//Enable the handbrake for the rear wheels only.
				wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = vehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = vehicleSettings.GetHandBrakeTorque();
			}
			else if (vehicleSettings.GetHandBrake() == HandBrake::FrontWheelsOnly)
			{
				//Enable the handbrake for the Front wheels only.
				wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxHandBrakeTorque = vehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxHandBrakeTorque = vehicleSettings.GetHandBrakeTorque();
			}
			else if (vehicleSettings.GetHandBrake() == HandBrake::RearWheelsOnly)
			{
				//Enable the handbrake for the all wheels.
				wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = vehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = vehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxHandBrakeTorque = vehicleSettings.GetHandBrakeTorque();
				wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxHandBrakeTorque = vehicleSettings.GetHandBrakeTorque();
			}

			//Enable steering for the front wheels only.
			wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = physx::PxPi * 0.3333f;
			wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = physx::PxPi * 0.3333f;
		}

		//Set up the tires.
		physx::PxVehicleTireData tires[PX_MAX_NB_WHEELS];
		{
			//Set up the tires.
			for (auto i = 0; i < vehicleSettings.GetNumberOfWheels(); i++)
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
			(vehicleSettings.GetNumberOfWheels(), wheelCenterActorOffsets,
				chassisCMOffset, vehicleSettings.GetChassisMass(), 1, suspSprungMasses);

			//Set the suspension data.
			for (auto i = 0; i < vehicleSettings.GetNumberOfWheels(); i++)
			{
				suspensions[i] = vehicleSettings.GetSuspension(i);
				suspensions[i].mSprungMass = suspSprungMasses[i];
			}

			//Set the camber angles.
			const physx::PxF32 camberAngleAtRest = 0.0;
			const physx::PxF32 camberAngleAtMaxDroop = 0.01f;
			const physx::PxF32 camberAngleAtMaxCompression = -0.01f;
			for (auto i = 0; i < vehicleSettings.GetNumberOfWheels(); i += 2)
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
			for (auto i = 0; i < vehicleSettings.GetNumberOfWheels(); i++)
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
		for (auto i = 0; i < vehicleSettings.GetNumberOfWheels(); i++)
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

		if (vehicleSettings.GetAntiRollBar() == AntiRollBar::Both)
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
		else if (vehicleSettings.GetAntiRollBar() == AntiRollBar::FrontOnly)
		{
			//Add a front and rear anti-roll bar
			physx::PxVehicleAntiRollBarData barFront;
			barFront.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
			barFront.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
			barFront.mStiffness = 10000.0f;
			wheelsSimData->addAntiRollBarData(barFront);
		}
		else if (vehicleSettings.GetAntiRollBar() == AntiRollBar::RearOnly)
		{

			physx::PxVehicleAntiRollBarData barRear;
			barRear.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT;
			barRear.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
			barRear.mStiffness = 10000.0f;
			wheelsSimData->addAntiRollBarData(barRear);
		}
    }
	physx::PxConvexMesh* CreateConvexMesh(IPhysics* physics, const physx::PxVec3* verts, const physx::PxU32 numVerts)
	{
		// Create descriptor for convex mesh
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = numVerts;
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = verts;
		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxConvexMesh* convexMesh = NULL;
		physx::PxDefaultMemoryOutputStream buf;
		if (physics->GetCooking()->cookConvexMesh(convexDesc, buf))
		{
			physx::PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
			convexMesh = physics->GetPhysics()->createConvexMesh(id);
		}

		return convexMesh;
	}
	void MakeWheelWidthsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii)
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
	physx::PxVec3 MakeChassis(physx::PxConvexMesh* chassisConvexMesh)
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
}