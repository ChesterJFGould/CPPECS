#pragma once
#include <EASTL/vector.h>
#include <EASTL/tuple.h>
#include <EASTL/unordered_map.h>
#include <EASTL/utility.h>
#include "SystemBase.h"
#include "Component.h"
#include "Types.h"

namespace ECS
{	
	template <class ... Comps>
	class System : public SystemBase
	{
	public:
		SystemID ID;

		System();
		~System();

		virtual void OnComponentAdded(const Entity& entity) override final;
		virtual void OnComponentRemoved(const Entity& entity) override final;
	protected:
		using ComponentTuple = eastl::tuple<Comps* ...>;
		eastl::vector<ComponentTuple> components;
	private:
		template <int tupleIndex, class CurrentComp, class ... OtherComps>
		bool checkIfEntityMatches(const Entity& entity, ComponentTuple& componentTuple) const;
		
		template <int tupleIndex>
		bool checkIfEntityMatches(const Entity& entity, ComponentTuple& componentTuple) const;
		
		eastl::unordered_map<EntityID, size_t> entityIDToIndex;
		eastl::vector<EntityID> indexToEntityID;
	};

	template <class ... Comps>
	void System<Comps ...>::OnComponentAdded(const Entity& entity)
	{
		ComponentTuple componentTuple;
		
		if (checkIfEntityMatches<0, Comps ...>(entity, componentTuple))
		{
			// Check if entity is already contained in components
			if (this->entityIDToIndex.find(entity.ID) != this->entityIDToIndex.end())
			{
				this->components[this->entityIDToIndex[entity.ID]] = componentTuple;
			}
			else
			{
				this->entityIDToIndex[entity.ID] = this->components.size();
				this->indexToEntityID.push_back(entity.ID);
				this->components.push_back(componentTuple);
			}
			
		}
	}

	template <class ... Comps>
	System<Comps ...>::System()
	{
		
	}

	template <class ... Comps>
	System<Comps ...>::~System()
	{
		
	}

	template <class ... Comps>
	void System<Comps ...>::OnComponentRemoved(const Entity& entity)
	{
		// Check if we care
		if (this->entityIDToIndex.find(entity.ID) != this->entityIDToIndex.end())
		{
			size_t entityIndex = this->entityIDToIndex[entity.ID];
			
			// Constant time remove entity from Components and indexToEntityID
			eastl::swap(this->components[entityIndex], this->components.back());
			this->components.pop_back();
			
			eastl::swap(this->indexToEntityID[entityIndex], this->indexToEntityID.back());
			this->indexToEntityID.pop_back();
			
			this->entityIDToIndex.erase(entity.ID);
			
			// Update entityIDToIndex to reflect swapped entity new position
			this->entityIDToIndex[this->indexToEntityID[entityIndex]] = entityIndex;
		}
	}

	// Recursive case: more than one type argument specified
	template <class ... Comps>
	template <int tupleIndex, class CurrentComp, class ... OtherComps>
	bool System<Comps ...>::checkIfEntityMatches(const Entity& entity, ComponentTuple& componentTuple) const
	{
		auto componentPair = entity.Components.find(CurrentComp::ID);
		if (componentPair == entity.Components.end())
		{
			return false;
		}
		else
		{
			eastl::get<tupleIndex>(componentTuple) = static_cast<CurrentComp*>(componentPair->second);
			return checkIfEntityMatches<tupleIndex+1, OtherComps ...>(entity, componentTuple);
		}
	}

	// Base case: final case after all types have been matched
	template <class ... Comps>
	template <int tupleIndex>
	bool System<Comps ...>::checkIfEntityMatches(const Entity& entity, ComponentTuple& componentTuple) const
	{
		return true;
	}
}
