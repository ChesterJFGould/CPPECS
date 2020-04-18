#pragma once
#include <EASTL/unordered_map.h>
#include "Component.h"
#include "Types.h"

namespace ECS
{
	class Entity
	{
	public:
		EntityID ID;
		eastl::unordered_map<ComponentID, Component*> Components;

		Entity();
		~Entity();
		
		bool Init(EntityID id);
	};
}



