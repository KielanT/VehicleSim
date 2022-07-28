#pragma once
#include <PxPhysicsAPI.h>

class UserErrorCallback : public physx::PxErrorCallback
{
public:
	virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file,
		int line)
	{
		// error processing implementation
	}
};

static UserErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;

PX_FORCE_INLINE physx::PxSimulationFilterShader getSampleFilterShader()
{
	return physx::PxDefaultSimulationFilterShader;
}

