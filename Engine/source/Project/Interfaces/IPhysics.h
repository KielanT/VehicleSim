#pragma once

#include "Project/Core.h"
#include "Math/CVector3.h"
#include <PxPhysicsAPI.h>


namespace Project
{
	

	class P_API IPhysics
	{
	public:
		virtual ~IPhysics(){}

		virtual bool InitPhysics(float meshWeldTolerance = 0.001f, CVector3 gravity = CVector3(0.0f, -9.81f, 0.0f)) = 0;
		virtual bool InitPhysics(physx::PxSimulationEventCallback* callBack, physx::PxSimulationFilterShader filterShader, float meshWeldTolerance = 0.001f, CVector3 gravity = CVector3(0.0f, -9.81f, 0.0f)) = 0;

		virtual void ShutdownPhysics() = 0;

		virtual const EPhysicsType GetPhysicsType() = 0;

		// TODO remove these functions
		virtual physx::PxPhysics* GetPhysics() = 0;
		virtual physx::PxCooking* GetCooking() = 0;
		virtual physx::PxScene* GetScene() = 0;

		
	};

	IPhysics* NewPhysics(const EPhysicsType type);

}
