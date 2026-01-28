
#ifndef ECS_H
#define ECS_H

#include "AEEngine.h"
#include "vector"
#include "array"
#include "queue"

#define MAX_ENTITY 20
#define MAX_COMPONENT 16

namespace ECSystem
{

	class Entity
	{
		public:
		u32 id;
		Entity() = default;
	};


	typedef enum
	{
		COMPONENT_TRANSFORM = 0,
		COMPONENT_VELOCITY = 1,
		COMPONENT_RENDERABLE = 2,
		COMPONENT_MESH = 3,
	} ComponentType;



	class Component
	{
		public:
		virtual ~Component() = default;
	};

	class TransformComponent : public Component
	{
		public:
		AEVec2 pos;
		AEVec2 size;
		f32 rotation;
		AEMtx33 transform;
		TransformComponent() = default;
		TransformComponent(f32 x, f32 y, f32 sx, f32 sy, f32 rot);
	};

	class Velocity : public Component
	{
		public:
		AEVec2 v;
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
		Entity Entity_new();
		void Entity_free(Entity e);
		void ECS_add_component(const Entity& entity, ComponentType type, void* data, size_t size);
		void* ECS_get_component(const Entity& entity, ComponentType type);
		ComponentPool* ECS_get_pool(ComponentType type);
		void ECS_remove_component(const Entity& entity, ComponentType type);

	};

}
#endif // !ECS_H