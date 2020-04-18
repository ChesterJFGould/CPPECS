#include "World.h"
#include "Types.h"

namespace ECS
{
	World::World() : nextEntityID(0), nextComponentID(0), nextSystemID(0)
	{
		
	}

	World::~World()
	{
		// TODO: Delete all heap allocated stuff9
	}

	EntityID World::CreateEntity()
	{
		this->Entities[this->nextEntityID] = Entity();
		
		return this->nextEntityID++;
	}

	void World::RemoveComponent(EntityID entityID, ComponentID componentID)
	{
		auto entityIt = this->Entities.find(entityID);
		if (entityIt == this->Entities.end())
		{
			// TODO: Probably should do some error handling here
			return;
		}
		
		Entity &entity = entityIt->second;
		
		auto componentIt = entity.Components.find(componentID);
		if (componentIt == entity.Components.end())
		{
			// TODO: Probably should do some error handling here
			return;
		}
		
		entity.Components.erase(componentID);
		
		for (SystemID& systemID : this->SystemInterests[componentID])
		{
			this->Systems[systemID]->OnComponentRemoved(entity);
		}
	}

	bool World::Init()
	{
		return false;
	}

	void World::Update(float frameTime)
	{
		for (SystemBase *system : this->Systems)
		{
			system->Update(frameTime);
		}
	}
}
