#include "pch.h"

#include "AnimationSystem.h"
#include "../system/GridSystem.h"

namespace Animation
{

	AnimationSystem::AnimationSystem()
	{

	}


	void idle_animation(EntityComponent::Registry& ecs, Entity id, Entity timer_id)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);
		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);


		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		transform->pos_onscreen.y = transform->pos.y + lerp * minimum * transform->size.y / 6;

	}


	bool moving_animation(EntityComponent::Registry& ecs, Entity id, Entity timer_id, f32 offset_x, f32 offset_y)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);
		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);
		Components::AStarResult* astar = ecs.getComponent<Components::AStarResult>(id);


		bool flag = false;

		if (!astar->path.empty())
		{
			Components::GridCell current = astar->path.front();

			if (timer->seconds >= timer->max_seconds)
			{

				transform->pos.x = offset_x + (current.x - current.y) * CELL_WIDTH / 2;
				transform->pos.y = transform->size.y / 3 + offset_y - (current.x + current.y) * CELL_HEIGHT / 4;
				astar->path.pop_front();
			}

			f32 lerp = timer->seconds / timer->max_seconds;

			f32 destination_x = offset_x + (current.x - current.y) * CELL_WIDTH / 2;
			f32 destination_y = transform->size.y / 3 + offset_y - (current.x + current.y) * CELL_HEIGHT / 4;

			transform->pos_onscreen.x = transform->pos.x + (destination_x - transform->pos.x) * lerp;
			transform->pos_onscreen.y = transform->pos.y + (destination_y - transform->pos.y) * lerp;
			/*transform->pos.x = this->offset.x + (i - j) * CELL_WIDTH / 2;
					transform->pos.y = transform->size.y / 2 + this->offset.y - (i + j) * CELL_HEIGHT / 4;
					transform->pos.y = transform->size.y / 3 + this->offset.y - (i + j) * CELL_HEIGHT / 4;
					transform->pos_onscreen = transform->pos;*/
		}
		else
		{
			flag = true;
		}

		return flag;
	}
	Entity animationTimer(EntityComponent::Registry& ecs, f32 max, f32 current, bool start, bool reset)
	{
		Entity id = ecs.createEntity();
		Components::Timer timer{ max, current, start, reset};
		ecs.addComponent(id, timer);
		return id;
	}

	void AnimationSystem::init(EntityComponent::Registry& ecs)
	{
		EntityComponent::ComponentTypeID animID = EntityComponent::getComponentTypeID<Components::Animation_Actor>();
		//create bitsets
		EntityComponent::ComponentBitMask objMask;
		objMask.set(animID);

		for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				for (Entity ent : it->second)
				{
					Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(ent);
					
					anim->timer_array[static_cast<size_t>(Components::AnimationType::ATTACK_MELEE)]  = animationTimer(ecs, 1.f, 0.f, true, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::ATTACK_RANGE)]  = animationTimer(ecs, 1.f, 0.f, true, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::MOVING)]        = animationTimer(ecs, 0.3f, 0.f, true, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::IDLE)]          = animationTimer(ecs, 1.f, 0.f, true, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::TAKING_DAMAGE)] = animationTimer(ecs, 1.f, 0.f, true, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::NONE)]          = animationTimer(ecs, 1.f, 0.f, true, true);
				}
			}
		}
	}


	void AnimationSystem::update(EntityComponent::Registry& ecs, Grid::GameBoard& gb, CombatNameSpace::CombatSystem& cs)
	{
		EntityComponent::ComponentTypeID animID = EntityComponent::getComponentTypeID<Components::Animation_Actor>();
		EntityComponent::ComponentTypeID timerID = EntityComponent::getComponentTypeID<Components::Timer>();
		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		//create bitsets
		EntityComponent::ComponentBitMask objMask;
		objMask.set(animID); objMask.set(transID);objMask.set(timerID);

		for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				for (Entity ent : it->second)
				{
					Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(ent);
					switch (anim->anim_type)
					{
						case Components::AnimationType::IDLE:
							idle_animation(ecs, ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::IDLE)]);
							break;
						case Components::AnimationType::MOVING:
							AEVec2 offset{ gb.GetOffsetPos() };
							if (moving_animation(ecs, ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::MOVING)], offset.x, offset.y))
							{
								anim->anim_type = Components::AnimationType::IDLE;
								cs.end_player_resolution();
							}
							break;
					}
				}
			}
		}
	}

}