#pragma once
#include "Project/Interfaces/IPhysics.h"
#include "PhysxHelpers.h"

namespace Project
{


	class PhysX : public IPhysics
	{
	public:
		virtual bool InitPhysics(float meshWeldTolerance = 0.001f, CVector3 gravity = CVector3(0.0f, -9.81f, 0.0f)) override;

		virtual void ShutdownPhysics() override;

		virtual const EPhysicsType GetPhysicsType() override;

		virtual physx::PxPhysics* GetPhysics() override { return m_Physics; } // TODO Remove this from here and interface
		virtual physx::PxScene* GetScene() override { return m_Scene; } // TODO Remove this from here and interface
		

	private:
		ErrorLogger m_Log;

		

		physx::PxFoundation* m_Foundation = nullptr;
		physx::PxPvd* m_Pvd = nullptr;
		physx::PxPhysics* m_Physics = nullptr;
		physx::PxCooking* m_Cooking = nullptr;

		physx::PxScene* m_Scene = nullptr;
		physx::PxDefaultCpuDispatcher* m_CpuDispatcher = nullptr;
	};
}

