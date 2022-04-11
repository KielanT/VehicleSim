#pragma once
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"

namespace Project
{
	class BasicVehicleComponent : public EntityComponent
	{
	public:
		BasicVehicleComponent(Entity* entity, TEntityUID UID);

	private:

	};
}
