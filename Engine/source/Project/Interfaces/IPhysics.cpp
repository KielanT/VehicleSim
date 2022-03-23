#include "ppch.h"
#include "IPhysics.h"
#include "Physics/PhysX4.1/PhysX4.h"

namespace Project
{
	IPhysics* NewPhysics(const EPhysicsType type)
	{
		if (type == EPhysicsType::PhysX4) // Returns the DirectX 11 renderer
		{
			return new PhysX();
		}
		else
		{
			return nullptr;
		}
	}
}