
#ifndef ECS_H
#define ECS_H

#include "AEEngine.h"
#include "vector"
#include "array"
#include "queue"

#define MAX_ENTITY 100
#define MAX_COMPONENT 16

namespace ECSystem
{

	class Entity
	{
		public:
		u32 id;
		s8 z;
		Entity() = default;
	};

	//ID FOR EACH OF THE COMPONENT
	typedef enum
	{
		COMPONENT_TRANSFORM = 0,
		COMPONENT_MESH = 1,
		COMPONENT_TEXTURE = 2,
		COMPONENT_GRID = 3,
	} ComponentType;



	class Component
	{
		public:
		virtual ~Component() = default;
	};

	class ComponentPool
	{
		public:
		std::array<void*, MAX_ENTITY> data;
		std::array<bool, MAX_ENTITY> has;
		int count;
	};

	class ECS
	{
		private:
		std::array<ComponentPool, MAX_COMPONENT> list{};
		std::queue<u32> free_ids;


		public:
		ECS();
		~ECS();
		Entity Entity_new(s8 z);
		void Entity_free(Entity e);
		void ECS_add_component(const Entity& entity, ComponentType type, void* data, size_t size);
		void* ECS_get_component(const Entity& entity, ComponentType type);
		ComponentPool* ECS_get_pool(ComponentType type);
		void ECS_remove_component(const Entity& entity, ComponentType type);

	};

}
#endif // !ECS_H