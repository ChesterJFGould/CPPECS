#pragma once
#include "Entity.h"

namespace ECS
{
	class SystemBase
	{
	public:
		SystemBase();
		~SystemBase();
		
		virtual void Update(float frameTime) = 0;
		virtual void OnComponentAdded(const Entity& entity) = 0;
		virtual void OnComponentRemoved(const Entity& entity) = 0;
	};
}
