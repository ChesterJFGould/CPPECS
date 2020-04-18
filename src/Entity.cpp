#include "Entity.h"

namespace ECS
{
	Entity::Entity()
	{
	}

	Entity::~Entity()
	{
	}

	bool Entity::Init(EntityID id)
	{
		this->ID = id;
		
		return true;
	}
}
