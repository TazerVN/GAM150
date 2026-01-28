#include "ecs.h"
#include "vector"
#include "cstdlib"


namespace ECSystem
{

	TransformComponent::TransformComponent(f32 x, f32 y, f32 sx, f32 sy, f32 rot)
	{
		this->pos.x = x;
		this->pos.y = y;
		this->size.x = sx;
		this->size.y = sy;
		this->rotation = rot;
		AEMtx33Identity(&this->transform);
	}


	ECS::ECS()
	{
		for (ComponentPool pool : this->list)
		{
			pool.count = 0;
			for (int i = 0; i < MAX_ENTITY; i++)
			{
				pool.data[i] = nullptr;
				pool.has[i] = false;
			}
		}
	}

	Entity ECS::ECS::Entity_new()
	{
		static u32 next_id = 0;
		u32 id;
		if (!this->free_ids.empty())
		{
			id = this->free_ids.front(); this->free_ids.pop();
		}
		else if (next_id < MAX_ENTITY)
		{
			id = next_id++;
		}
		else
		{
			return { MAX_ENTITY };  // Invalid
		}
		Entity p{id};
		return p;
	}

	void ECS::ECS::Entity_free(Entity e)
	{
		if (e.id < MAX_ENTITY)
		{
			free_ids.push(e.id);  // Recycle
		}
	}




	void ECS::ECS_add_component(const Entity& entity, ComponentType type, void* data, size_t size)
	{
		if (entity.id >= MAX_ENTITY || type >= MAX_COMPONENT) return;
		ComponentPool& pool = this->list[type];
		void* copy = malloc(size);

		if (!copy) return;

		memcpy(copy, data, size);
		free(pool.data[entity.id]);
		pool.data[entity.id] = copy;
		pool.has[entity.id] = true;
		if(!pool.has[entity.id]) pool.count++;

	}

	void* ECS::ECS_get_component(const Entity&  entity, ComponentType type)
	{
		if (entity.id >= MAX_ENTITY || type >= MAX_COMPONENT) return nullptr;
		if (this->list[type].has[entity.id] == true) return this->list[type].data[entity.id];
	}

	ComponentPool* ECS::ECS_get_pool(ComponentType type)
	{
		if(type >= MAX_COMPONENT) return nullptr;
		return &this->list[type];
	}

	void ECS::ECS_remove_component(const Entity& entity, ComponentType type)
	{
		if (entity.id >= MAX_ENTITY || type >= MAX_COMPONENT) return;
		if (this->list[type].has[entity.id] == true)
		{
			this->list[type].has[entity.id] = false;
			this->list[type].data[entity.id] = nullptr;
			this->list[type].count--;
		}
	}


}