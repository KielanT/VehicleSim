#include "ppch.h"
#include "VehicleSettings.h"

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
        data.mCamberAtRest = 0.0f;
        data.mCamberAtMaxDroop = 0.001f;
        data.mCamberAtMaxCompression = 0.001f;
        return data;
    }
}