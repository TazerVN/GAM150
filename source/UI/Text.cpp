//=========================================
//	AUTHOR:		PHAM MINH TUAN
//	EMAIL:		minhtuan.pham@digipen.edu
//	DATE:		5-4-2026
//=========================================
#include "pch.h"
#include "Text.h"

namespace Text
{
	InstructionText& operator<<(InstructionText& lhs, const char* text)
	{
		lhs.free();
		f32 w,h;
		AEGfxGetPrintSize(TF.getFontID(), text, lhs.size, &w, &h);

		lhs.text.push_back( new UIO::TextShadow{ w * AEGfxGetWinMinX() * 0.5f , lhs.y + h * AEGfxGetWinMinY(), lhs.size, lhs.z, text, lhs.rgba });

		//lhs.text = UIO::TextShadow{ w * AEGfxGetWinMinX() * 0.5f , h * AEGfxGetWinMinY() + lhs.y, lhs.size, lhs.z, text, lhs.rgba};
		return lhs;
	}

	void InstructionText::set(f32 _y, f32 _size)
	{
		this->y = _y;
		this->size = _size;
	}

	void InstructionText::set(Components::RGBA color)
	{
		this->rgba = color;
	}

	InstructionText& InstructionText::operator+=(const char* _text)
	{
		f32 gap = 50.f;
		f32 w, h;
		AEGfxGetPrintSize(TF.getFontID(), _text , this->size, &w, &h);
		this->text.push_back(new UIO::TextShadow{ w * AEGfxGetWinMinX() * 0.5f , this->y + (h * AEGfxGetWinMinY()) - gap * (this->text.size()), this->size, this->z, _text, this->rgba });

		//lhs.text = UIO::TextShadow{ w * AEGfxGetWinMinX() * 0.5f , h * AEGfxGetWinMinY() + lhs.y, lhs.size, lhs.z, text, lhs.rgba};

		return *this;
	}



	void InstructionText::free()
	{

		for(UIO::TextShadow* pt : this->text)
		{
			pt->free();
			delete pt;
		}
		text.clear();
	}

	void PopUpText::display(Entity _pos, const char* _text)
	{
		//*this << 0;
		this->current.push_back(this->text_create_attached(_pos, _text, c_color.r, c_color.g, c_color.b, c_color.a));
	}

	PopUpText& operator<<(PopUpText& out, const char* _text)
	{
		Entity id = out.text_create(out.pos, _text, out.c_color.r, out.c_color.g, out.c_color.b, out.c_color.a);

		out.current.push_back(id);

		return out;
	}
	PopUpText& operator<<(PopUpText& out, s32 value)
	{
		switch(value)
		{
			case 0:
				out.c_color = {1.0f, 0.2f, 0.2f, 1.f};
				break;
			case 1:
				out.c_color = {0.5f, 0.5f, 1.0f, 1.f };
				break;
			case 2:
				out.c_color = { 1.0f, 1.0f, 1.0f, 1.f};
				break;
			default:
				break;

		}
		return out;
	}

	void PopUpText::init(EntityComponent::Registry* ecsptr, Entity loc)
	{
		this->ecs = ecsptr;
		this->pos = loc;
	}

	Entity PopUpText::text_create(Entity e, const char* t, f32 r, f32 g, f32 b, f32 a)
	{
		Entity id = this->ecs->createEntity();

		Components::Transform* poos = ecs->getComponent<Components::Transform>(e);

		/*f32 x = pos->pos_onscreen.x - 64.f;
		f32 y = pos->pos_onscreen.y + 32.f;*/
		f32 x = poos->pos_onscreen.x - 100.f;
		f32 y = poos->pos_onscreen.y + 150.f;

		

		Components::Transform trans{ {x, y}, {x,y} ,{0.5f, 0.5f} , {0.5f, 0.5f} ,0.0f };
		Components::Text text{ t, TF.getFontID(), this->z };
		Components::Color color{ r, g, b ,a};
		Components::Timer time{ 3.f, 0.f };
		Components::Velocity vel{ 0.f, 1.f };
		Components::TagClass tag{ Components::Tag::UI };

		ecs->addComponent(id, tag);
		ecs->addComponent(id, trans);
		ecs->addComponent(id, text);
		ecs->addComponent(id, color);
		ecs->addComponent(id, time);
		ecs->addComponent(id, vel);
		return id;
	}

	Entity PopUpText::text_create_attached(Entity e, const char* t, f32 r, f32 g, f32 b, f32 a)
	{
		Entity id = this->ecs->createEntity();

		Components::Transform* ppos = ecs->getComponent<Components::Transform>(e);

		f32 x{}, y{};

		if (ppos != nullptr)
		{
			x = ppos->pos_onscreen.x;
			y = ppos->pos_onscreen.y + 100.f;
		}

		Components::Transform trans{ {x, y}, {x,y} ,{0.5f, 0.5f} , {0.5f, 0.5f} ,0.0f };
		Components::Text text{ t, TF.getFontID(), this->z };
		Components::Color color{ r, g, b ,a };
		Components::Timer time{ 2.f, 0.f };
		Components::Velocity vel{ 0.f, 1.f };

		ecs->addComponent(id, trans);
		ecs->addComponent(id, text);
		ecs->addComponent(id, color);
		ecs->addComponent(id, time);
		ecs->addComponent(id, vel);
		PUT << 0;
		return id;
	}

	void PopUpText::free()
	{
		for (Entity s : this->current)
		{
			this->ecs->destroyEntity(s);
		}
		this->current.clear();
	}

	Entity PopUpText::get() const
	{
		return this->pos;
	}

	void PopUpText::set(Entity id)
	{
		this->pos = id;
	}

	void PopUpText::update()
	{
		for (int i = 0; i < this->current.size(); i++)
		{
			Components::Timer* timer = ecs->getComponent<Components::Timer>(this->current[i]);
			Components::Color* c = ecs->getComponent<Components::Color>(this->current[i]);
			Components::Velocity* vel = ecs->getComponent<Components::Velocity>(this->current[i]);


			f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
			//f32 minimum = 0.4f;


			c->d_color.a = lerp;
			vel->vel.y = 10.0f;





			if (timer->seconds >= timer->max_seconds)
			{
				if (this->current.size() == 1)
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



	void NameTag::create_static_nametag(Entity parent, const char* a)
	{
		Entity id = ecs.createEntity();

		Components::Transform trans{ {0, 0}, {0,0} ,{0.3f, 0.5f} , {0.3f, 0.5f} ,0.0f };
		Components::Text text{ a, TF.getFontID(), 25 };
		Components::Color color{ 1.0f, 0.0f, 0.0f ,1.0f };
		Components::Timer time{ 2.f, 0.f };
		Components::Velocity vel{ 0.f, 1.f };

		ecs.addComponent(id, trans);
		ecs.addComponent(id, text);
		ecs.addComponent(id, color);
		ecs.addComponent(id, time);
		ecs.addComponent(id, vel);

		nametag.push_back({ id, parent });
	}
	void NameTag::update()
	{
		for (std::pair<Entity, Entity> text : nametag)
		{
			Entity cur = text.first;
			Entity parent = text.second;

			Components::Transform* parent_pos = ecs.getComponent<Components::Transform>(parent);

			f32 x = parent_pos->pos.x - 64.f;
			f32 y = parent_pos->pos.y + 32.f;
			f32 psx = parent_pos->pos_onscreen.x - 64.f;
			f32 psy = parent_pos->pos_onscreen.y + 32.f;
			ecs.getComponent<Components::Transform>(cur)->pos.x = x;
			ecs.getComponent<Components::Transform>(cur)->pos.y = y;
			ecs.getComponent<Components::Transform>(cur)->pos_onscreen.x = psx;
			ecs.getComponent<Components::Transform>(cur)->pos_onscreen.y = psy;
		}
	}
	void NameTag::name_tag_free()
	{
		for (std::pair<Entity, Entity> a : nametag)
		{
			ecs.destroyEntity(a.first);
		}
		nametag.clear();
	}
}