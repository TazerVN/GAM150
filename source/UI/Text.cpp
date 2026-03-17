#include "pch.h"
#include "Text.h"

namespace Text{

PopUpText& operator<<(PopUpText& out, const char* text)
{
	out.current.push_back(out.text_create(text));
	return out;
}

void PopUpText::init(EntityComponent::Registry* ecs, Entity pos)
{
	this->ecs = ecs;
	this->pos = pos;
}

Entity PopUpText::text_create(const char* a)
{
	Entity id = this->ecs->createEntity();

	Components::Transform* pos = ecs->getComponent<Components::Transform>(this->pos);

	f32 x = pos->pos_onscreen.x - 64.f;
	f32 y = pos->pos_onscreen.y + 32.f;

	Components::Transform trans{ {x, y}, {x,y} ,{0.3f, 0.5f} , {0.3f, 0.5f} ,0.0f };
	Components::Text text{ a, TF.getFontID(), 25 };
	Components::Color color{ 1.0f, 0.0f, 0.0f ,1.0f };
	Components::Timer time{2.f, 0.f};
	Components::Velocity vel{0.f, 1.f};

	ecs->addComponent(id, trans);
	ecs->addComponent(id, text);
	ecs->addComponent(id, color);
	ecs->addComponent(id, time);
	ecs->addComponent(id, vel);
	return id;
}

void PopUpText::update()
{
	for(int i = 0; i < this->current.size(); i++)
	{
		Components::Timer* timer = ecs->getComponent<Components::Timer>(this->current[i]);
		Components::Color* c = ecs->getComponent<Components::Color>(this->current[i]);
		Components::Velocity* vel = ecs->getComponent<Components::Velocity>(this->current[i]);


		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.4f;


		c->d_color.a = lerp;
		vel->vel.y = 10.0f;
	

	


		if(timer->seconds >= timer->max_seconds)
		{
			if(this->current.size() == 1)
			{
				this->ecs->destroyEntity(this->current[i]);
				this->current.pop_back();
				break;
			}
			else
			{
				this->ecs->destroyEntity(this->current[i]);
				this->current[i] = this->current[this->current.size() - 1];
				this->current.pop_back();
				break;
			}
		}
	}
}
}