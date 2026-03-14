#include "pch.h"

#include "AnimationSystem.h"

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

		transform->pos_onscreen.y =  transform->pos.y + lerp * minimum * transform->size.y / 6;
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
					for(int i = 0; i < anim->timer_array.size(); i++){
						Components::Timer timer{1.f, 0.5f, true, true};
						anim->timer_array[i] = ecs.createEntity();
						ecs.addComponent(anim->timer_array[i], timer);
					}
				}
			}
		}
	}


	void AnimationSystem::update(EntityComponent::Registry& ecs)
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
							break;
					}
				}
			}
		}
	}

}