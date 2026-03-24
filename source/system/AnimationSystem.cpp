#include "pch.h"

#include "AnimationSystem.h"
#include "../system/GridSystem.h"
namespace Animation
{

	AnimationSystem::AnimationSystem()
	{

	}

	bool range_animation(Entity id, Entity timer_id)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);

		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);
		Components::Texture* texture = ecs.getComponent<Components::Texture>(id);

		timer->start = true;
		f32 lerp = timer->seconds / timer->max_seconds;
		bool flag = false;

		if (lerp >= 1.f)
		{
			flag = true;
			texture->offset_x = 0.f;
			timer->start = false;
		}
		else
		{
			texture->offset_x = 13.f / 18.f;
		}

		return flag;
	}

	void idle_animation(Entity id, Entity timer_id)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);
		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);

		timer->start = true;
		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		transform->pos_onscreen.y = transform->pos.y + lerp * minimum * transform->size.y / 6;

	}

	bool melee_animation(Entity id, Entity timer_id)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);

		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);
		Components::Texture* texture = ecs.getComponent<Components::Texture>(id);
		Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(id);

		bool flag = false;

		timer->start = true;
		f32 lerp = timer->seconds / timer->max_seconds;
		f32 fps = timer->max_seconds / 6.f;



		if (lerp >= 1.f)
		{
			texture->offset_x = 0.f;
			flag = true;
			timer->start = false;
			anim->current_frame = 0.f;
		}
		else
		{

			int frame = 7 + int(lerp * 5);
			if (static_cast<int>(timer->seconds * 100) % static_cast<int>(fps * 100) == 0)
				anim->current_frame = ++anim->current_frame % anim->max_frame;

			texture->offset_x = frame / 18.f;
			/*Components::Transform* camera = ecs.getComponent<Components::Transform>(CS.id());
			int magnitude = 10;
			camera->pos.x = camera->pos_onscreen.x + magnitude / 2 - AERandFloat() * magnitude;
			camera->pos.y = camera->pos_onscreen.y + magnitude / 2 - AERandFloat() * magnitude;*/
		}

		return flag;
	}


	bool moving_animation(Entity id, Entity timer_id, f32 offset_x, f32 offset_y)
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
			f32 fps = (timer->max_seconds / 6.f);

			if ((destination_y - transform->pos.y) > 0 && (destination_x - transform->pos.x) < 0)
			{
				texture->offset_y = 3.f / 4.f;
			}
			else if ((destination_y - transform->pos.y) < 0 && (destination_x - transform->pos.x) < 0)
			{
				texture->offset_y = 1.f;
			}
			else if ((destination_y - transform->pos.y) > 0 && (destination_x - transform->pos.x) > 0)
			{
				texture->offset_y = 2.f / 4.f;
			}
			else if ((destination_y - transform->pos.y) < 0 && (destination_x - transform->pos.x) > 0)
			{
				texture->offset_y = 1.f / 4.f;
			}

			
			int frame = 1 + int(lerp * 5);


			if (static_cast<int>(timer->seconds * 100) % static_cast<int>(fps * 100) == 0 &&
				(transform->pos.x != transform->pos_onscreen.x && transform->pos.y != transform->pos_onscreen.y))
				texture->offset_x = frame / 18.f;

			transform->pos_onscreen.x = transform->pos.x + (destination_x - transform->pos.x) * lerp;
			transform->pos_onscreen.y = transform->pos.y + (destination_y - transform->pos.y) * lerp;
			mesh->z = current.x + 2;

		}
		else
		{
			flag = true;
			timer->start = false;
			texture->offset_x = 0;
		}

		return flag;
	}
	bool moving_animation_enemy(Entity id, Entity timer_id, f32 offset_x, f32 offset_y)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);
		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);
		Components::AStarResult* astar = ecs.getComponent<Components::AStarResult>(id);
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

	bool range_animation_enemy(Entity id, Entity timer_id)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);
		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);
		Components::Color* color = ecs.getComponent<Components::Color>(id);

		bool flag = false;
		timer->start = true;

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;


		if (timer->seconds >= timer->max_seconds)
		{
			flag = true;
			timer->start = false;
		}
		else
		{
			transform->pos_onscreen.x = transform->pos.x + AERandFloat() * 10;
		}

		return flag;
	}

	bool damage_animation(Entity id, Entity timer_id)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);
		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);
		Components::Color* color = ecs.getComponent<Components::Color>(id);

		bool flag = false;
		timer->start = true;

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;


		if (timer->seconds >= timer->max_seconds)
		{
			flag = true;
			timer->start = false;
			color->d_color = color->c_color;

			transform->pos_onscreen.x = transform->pos.x;
		}
		else
		{
			transform->pos_onscreen.x = transform->pos.x + AERandFloat() * 10;
			color->d_color.r = 1.f + AERandFloat();
			color->d_color.b = 1.f + AERandFloat();
			color->d_color.g = 1.f + AERandFloat();
		}

		return flag;
	}

	bool death_animation(Entity id, Entity timer_id)
	{
		Components::Timer* timer = ecs.getComponent<Components::Timer>(timer_id);
		Components::Transform* transform = ecs.getComponent<Components::Transform>(id);
		Components::Color* color = ecs.getComponent<Components::Color>(id);

		bool flag = false;
		timer->start = true;

		f32 lerp = timer->seconds / timer->max_seconds;
		f32 minimum = 0.6f;


		if (timer->seconds >= timer->max_seconds)
		{
			flag = true;
			timer->start = false;

		}
		else
		{
			color->d_color.a = 1.f - lerp;
		}

		return flag;
	}


	Entity animationTimer(EntityComponent::Registry& ecs, f32 max, f32 current, bool start, bool reset)
	{
		Entity id = ecs.createEntity();
		Components::Timer timer{ max, current, start, reset };
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

					anim->timer_array[static_cast<size_t>(Components::AnimationType::ATTACK_MELEE)] = animationTimer(ecs, 0.5f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::ATTACK_RANGE)] = animationTimer(ecs, 0.5f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::MOVING)] = animationTimer(ecs, 0.3f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::IDLE)] = animationTimer(ecs, 1.f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::TAKING_DAMAGE)] = animationTimer(ecs, 0.2f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::ENEMY_ATTACK)] = animationTimer(ecs, 0.2f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::ENEMY_MOVING)] = animationTimer(ecs, 0.1f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::DEATH)] = animationTimer(ecs, 0.5f, 0.f, false, true);
					anim->timer_array[static_cast<size_t>(Components::AnimationType::NONE)] = animationTimer(ecs, 1.f, 0.f, false, true);
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
					switch (anim->getCurrType())
					{
						case Components::AnimationType::IDLE:
							idle_animation(ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::IDLE)]);
							break;
						case Components::AnimationType::MOVING:
						{
							AEVec2 offset{ gb.GetOffsetPos() };
							if (moving_animation(ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::MOVING)], offset.x, offset.y))
							{
								anim->setType(anim->default_type);
								gbs.set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
							}
							break;
						}
						case Components::AnimationType::ATTACK_MELEE:
							if (melee_animation(ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::ATTACK_MELEE)]))
							{
								anim->setType(anim->default_type);
								cs.set_play_card_triggered(true);
							}
							break;
						case Components::AnimationType::ATTACK_RANGE:
							if (range_animation(ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::ATTACK_RANGE)]))
							{
								anim->setType(anim->default_type);
								cs.set_play_card_triggered(true);
							}
							break;
						case Components::AnimationType::ENEMY_MOVING:
							AEVec2 offset{ gb.GetOffsetPos() };
							if (moving_animation_enemy(ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::ENEMY_MOVING)], offset.x, offset.y))
							{
								anim->setType(anim->default_type);
								gbs.set_EnemyPhase(PhaseSystem::EnemyPhase::ENEMY_IDLE);
							}
							break;
						case Components::AnimationType::ENEMY_ATTACK:
							if (range_animation_enemy(ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::ENEMY_ATTACK)]))
							{
								anim->setType(anim->default_type);
								gbs.set_EnemyPhase(PhaseSystem::EnemyPhase::ENEMY_IDLE);
							}
							break;
						case Components::AnimationType::TAKING_DAMAGE:
							if (damage_animation(ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::TAKING_DAMAGE)]))
							{
								anim->setType(anim->default_type);
							}
							break;
						case Components::AnimationType::DEATH:
							if (death_animation(ent, anim->timer_array[static_cast<size_t>(Components::AnimationType::DEATH)]))
							{
								anim->setType(anim->default_type);
							}
							break;
					}
				}
			}
		}
	}

}