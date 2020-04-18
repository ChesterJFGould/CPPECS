#pragma once
#include <iostream>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>
#include <EASTL/unordered_set.h>
#include "SystemBase.h"
#include "Component.h"
#include "Types.h"
#include "Entity.h"
#include "EASTLFix.h"
#include "System.h"

namespace ECS
{
	class World
	{
	public:
		eastl::unordered_map<EntityID, Entity> Entities;
		eastl::vector<eastl::unordered_set<Component*>> Components;
		eastl::vector<SystemBase*> Systems;
		eastl::vector<eastl::vector<SystemID>> SystemInterests;
		
		World();
		~World();
		
		bool Init();
		
		EntityID CreateEntity();
		
		template <class ComponentType>
		void AddComponent(EntityID entityID, ComponentType* component);
		void RemoveComponent(EntityID entityID, ComponentID componentID);
		
		template <class ComponentType>
		void NewComponent();
		
		template <class ... Comps>
		void NewSystem(System<Comps ...> *system);
		
		void Update(float frameTime);
	private:
		EntityID nextEntityID;
		ComponentID nextComponentID;
		SystemID nextSystemID;
		template <int TimeSaver>
		bool addSystemInterests(SystemID systemID);
		
		template <int TimeSaver, class CurrentComp, class ... OtherComps>
		bool addSystemInterests(SystemID systemID);
	};

	template <class ComponentType>
	void World::AddComponent(EntityID entityID, ComponentType *component)
	{
		auto entityIt = this->Entities.find(entityID);
		if (entityIt == this->Entities.end())
		{
			// TODO: Probably should do some error handling here
			return;
		}
		
		Entity &entity = entityIt->second;
		
		entity.Components[ComponentType::ID] = component;
		
		this->Components[ComponentType::ID].insert(component);
		
		std::cout << this->SystemInterests.size() << std::endl;
		
		for (SystemID& systemID : this->SystemInterests[ComponentType::ID])
		{
			this->Systems[systemID]->OnComponentAdded(entity);
		}
	}

	template <class ComponentType>
	void World::NewComponent()
	{
		ComponentType::ID = this->nextComponentID++;
		
		eastl::unordered_set<Component*> compSet;
		this->Components.push_back(compSet);
		
		eastl::vector<SystemID> sysIDVec;
		
		this->SystemInterests.push_back(sysIDVec);
	}

	template <class ... Comps>
	void World::NewSystem(System<Comps ...> *system)
	{
		system->ID = this->nextSystemID++;
		
		this->Systems.push_back(system);
		
		if (!this->addSystemInterests<0, Comps ...>(system->ID))
		{
			// TODO: Error handling
		}	
	}

	template <int TimeSaver, class CurrentComp, class ... OtherComps>
	bool World::addSystemInterests(SystemID systemID)
	{
		if (CurrentComp::ID >= this->SystemInterests.size())
		{
			// TODO: Error handling
			return false;
		}
		
		this->SystemInterests[CurrentComp::ID].push_back(systemID);
		
		return this->addSystemInterests<TimeSaver, OtherComps ...>(systemID);
	}

	template <int TimeSaver>
	bool World::addSystemInterests(SystemID systemID)
	{
		return true;
	}
}
