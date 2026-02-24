#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <type_traits>
#include <memory>
#include <unordered_map>
#include <queue>
#include "Components.h"

using Entity = size_t;
//early declaration

namespace ECS
{
	using ComponentTypeID = size_t;
	using ComponentBitMask = std::bitset<MAX_COMPONENT>;

	inline ComponentTypeID getNextComponentTypeID()
	{
		static ComponentTypeID lastTypeID = 0;
		return lastTypeID++;
	}

	template <typename T>
	ComponentTypeID getComponentTypeID()
	{
		static ComponentTypeID typeID = getNextComponentTypeID();
		return typeID;
	}

	class IComponentStorage
	{
	public:
		virtual ~IComponentStorage() = default;
		virtual void delete_component(Entity e) = 0;
	};

	template<typename Component>
	class ComponentStorage : public IComponentStorage
	{
	public:
		void add_component(Entity e, Component component)
		{
			if (e >= sparse_set.size())
			{
				sparse_set.resize(e + 1, NULL_INDEX);
			}
			sparse_set[e] = dense_set.size();
			dense_set.push_back(component);
			dens2sparse.push_back(e);
		}
		size_t getCount()
		{
			return dense_set.size();
		}
		Component* get_component(Entity e)
		{
			size_t index = sparse_set[e];
			if (index != NULL_INDEX)
			{
				return &dense_set[index];
			}
			return nullptr;	//no component found!
		}
		void delete_component(Entity e) override
		{
			size_t index = sparse_set[e];
			if (index == NULL_INDEX) return;	//if no component to delete return
			//find entity id for back
			Entity backEntityID = dens2sparse.back();

			std::swap(dense_set[index], dense_set.back());	//swap target index to back
			std::swap(dense_set[index], dense_set.back());	//swap target index to back

			sparse_set[e] = NULL_INDEX;
			if (e != backEntityID)
			{
				sparse_set[backEntityID] = index;
				dens2sparse[index] = backEntityID;
			}
			dense_set.pop_back();
			dens2sparse.pop_back();
		}
	private:
		std::vector<Component> dense_set;
		std::vector<size_t> sparse_set;	//sparse set containing 
		std::vector<size_t>dens2sparse;	//reference for dense_set to sparse
	};

	class Registry {
	private:
		Entity nextEntity = 0;
		std::vector<ComponentBitMask> entitySignatures;
		std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentStorage>> componentStorage;
		std::queue<Entity> removed_que;
	public:

		std::vector<ComponentBitMask>& getBitMask()
		{
			return entitySignatures;
		}

		size_t sizeEntity(){
			return nextEntity;
		}

		Entity createEntity()
		{
			if (!removed_que.empty())
			{
				Entity e = removed_que.front();
				removed_que.pop();	//pops from the front
				return e;
			}
			Entity e = nextEntity++;
			entitySignatures.emplace_back();
			return e;
		}

		void destroyEntity(Entity e)
		{
			//i represent ComponentTypeID
			for (size_t i = 0; i < entitySignatures[e].size(); ++i)
			{
				if (entitySignatures[e].test(i))
				{
					removeComponent(e, i);
				}
			}
			entitySignatures[e].reset();
			removed_que.push(e);
		}

		template<typename T>
		void addComponent(Entity e, const T& component)
		{
			if (e >= entitySignatures.size()) return;

			ComponentTypeID typeID = getComponentTypeID<T>();

			std::unique_ptr<IComponentStorage>& baseStorage = componentStorage[typeID];

			if (!baseStorage)
			{
				std::cout << "No Base storage is there created one!\n";
				baseStorage = std::make_unique<ComponentStorage<T>>();
			}
			else std::cout << "Base storage is found \n";
			//get storage
			ComponentStorage<T>* storage = static_cast<ComponentStorage<T>*>(baseStorage.get());
			storage->add_component(e, component);
			entitySignatures[e].set(typeID);
		}
		//get code
		template<typename T>
		T* getComponent(Entity e)
		{
			if (e >= entitySignatures.size()) return nullptr;
			const ComponentTypeID typeID = getComponentTypeID<T>();

			if (!entitySignatures[e].test(typeID)) return nullptr;

			auto it = componentStorage.find(typeID);	//returns unordered_map.emd() if not found
			if (it == componentStorage.end()) return nullptr;
			//if the typeID is found cast find the pointer to the component storage
			ComponentStorage<T>* storage = static_cast<ComponentStorage<T>*>(it->second.get());

			if (storage) return storage->get_component(e);

			//if still not returned then return nullptr
			return nullptr;
		}
		//remove code 
		template<typename T>
		void removeComponent(Entity e)
		{
			const ComponentTypeID typeID = getComponentTypeID<T>();

			auto it = componentStorage.find(typeID);
			if (it == componentStorage.end())
				return;

			auto* storage =
				static_cast<ComponentStorage<T>*>(it->second.get());

			storage->delete_component(e);
			entitySignatures[e].reset(typeID);
		}

		void removeComponent(Entity e, const ComponentTypeID typeID)
		{
			auto it = componentStorage.find(typeID);
			if (it == componentStorage.end())
				return;

			auto* storage = it->second.get();
			storage->delete_component(e);
			entitySignatures[e].reset(typeID);
		}

		template<typename T>
		ComponentStorage<T>* getComponentStorage()
		{

			auto n = componentStorage.find(getComponentTypeID<T>());
			if (n == componentStorage.end()) return nullptr;
			return static_cast<ComponentStorage<T>*>(n->second.get());
		}

	};
};


