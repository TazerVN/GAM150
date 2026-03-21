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

		timer->start = true;
		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		transform->pos_onscreen.y = transform->pos.y + lerp * minimum * transform->size.y / 6;

	}

	bool melee_animation(EntityComponent::Registry& ecs, Entity id, Entity timer_id)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);

		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);
		Components::Texture* texture = ecs.getComponent<Components::Texture>(id);
		Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(id);

		bool flag = false;

		timer->start = true;
		f32 lerp = timer->seconds / timer->max_seconds;
		f32 fps = timer->max_seconds/6.f;

		if (lerp >= 1.f)
		{
			texture->offset_x = 0.f;
			texture->offset_y = 0.f;
			flag = true;
			timer->start = false;
			anim->current_frame = 0.f;
		}
		else
		{

			if (texture->offset_y >= 0.f && texture->offset_y < (1.f / 6.f))
			{
				texture->offset_y = 2.f / 6.f;
			}
			else if (texture->offset_y >= 1.f/6.f && texture->offset_y < (2.f / 6.f))
			{
				texture->offset_y = 3.f / 6.f;
			}

			if(static_cast<int>(timer->seconds * 100) % static_cast<int>(fps * 100) == 0)
				anim->current_frame = ++anim->current_frame % anim->max_frame;

			texture->offset_x = f32(anim->current_frame) / f32(anim->max_frame);
		}

		return flag;
	}


	bool moving_animation(EntityComponent::Registry& ecs, Entity id, Entity timer_id, f32 offset_x, f32 offset_y)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);
		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);
		Components::AStarResult* astar = ecs.getComponent<Components::AStarResult>(id);
		Components::Texture* texture = ecs.getComponent<Components::Texture>(id);
		Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(id);

		timer->start = true;
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

			if ((destination_y - transform->pos.y) > 0)
			{
				texture->offset_x = 1.f/6.f;
			}
			else if ((destination_y - transform->pos.y) < 0){
				texture->offset_x = 0;
			}

			transform->pos_onscreen.x = transform->pos.x + (destination_x - transform->pos.x) * lerp;
			transform->pos_onscreen.y = transform->pos.y + (destination_y - transform->pos.y) * lerp;
			mesh->z = current.x + 2;
	
		}
		else
		{
			flag = true;
			timer->start = false;
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
					
					anim->timer_array[static_cast<size_t>(Components::AnimationType::ATTACK_MELEE)]  = animationTimer(ecs, 1.f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::ATTACK_RANGE)]  = animationTimer(ecs, 1.f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::MOVING)]        = animationTimer(ecs, 0.2f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::IDLE)]          = animationTimer(ecs, 1.f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::TAKING_DAMAGE)] = animationTimer(ecs, 1.f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::NONE)]          = animationTimer(ecs, 1.f, 0.f, false, true);
				}
			}
		}
	}


	void AnimationSystem::update(EntityComponent::Registry& ecs, Grid::GameBoard& gb, PhaseSystem::GameBoardState& gbs, CombatNameSpace::CombatSystem& cs)
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
								anim->anim_type = Components::AnimationType::NONE;
								gbs.set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
							}
							break;
						case Components::AnimationType::ATTACK_MELEE:
							if (melee_animation(ecs, ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::ATTACK_MELEE)]))
							{
								anim->anim_type = Components::AnimationType::NONE;
								cs.set_play_card_triggered(true);
							}
							break;
					}
				}
			}
		}
	}

}