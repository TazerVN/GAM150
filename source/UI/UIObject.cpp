#include "pch.h"
#include "UIObject.h"

namespace UIO
{

	TextButton::TextButton(f32 x, f32 y, f32 width, f32 height, f32 text_size, f32 rotation, s32 z, const char* a, std::function<void()> func, Components::RGBA rgba)
	{
		this->z = z;
		this->button = ui_button(x, y, width, height, 0, z, func, {1.f - rgba.r, 1.f - rgba.g, 1.f - rgba.b, 1 - rgba.a });
		this->text = UIO::TextShadow{ x - 64.f * text_size, y - 64.f * text_size / 2.f, x - 64.f * text_size, y - 100.f * text_size / 2.f ,text_size, z + 1, a, {1.f, 1.f, 1.f, 1.f} };
	}

	void TextButton::free()
	{
		if(this->button != 0) ecs.destroyEntity(this->button);
		this->text.free();
		this->button = 0;
	}


	TextShadow::TextShadow(f32 x, f32 y, f32 s_x, f32 s_y ,f32 text_size, s32 z, const char* a, Components::RGBA rgba)
	{
		this->z = z;
		this->text = ui_interactive_text(x, y, text_size, text_size, 0, z + 1, a, { 1.f, 1.f, 1.f, 1.f });
		this->text_shadow = ui_text(s_x, s_y, text_size, text_size, 0, z + 1, a, { 0.f, 0.f, 0.f, 1.f });
	}
	void TextShadow::free()
	{
		if(this->text != 0) ecs.destroyEntity(this->text);
		if(this->text_shadow != 0) ecs.destroyEntity(this->text_shadow);
		this->text = 0;
		this->text_shadow = 0;
	}

	ScreenTransition::ScreenTransition(bool fadeIn) : z{1300}
	{
		this->fadeIn = fadeIn;
		this->finished = false;
		if(fadeIn){
			this->dim = ui_blank_solid_center(0, 0, AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), 0, this->z, 0.f, 0.f, 0.f, 1.f);
			Components::Timer timer{ 2.f, 0.f, true, false};
			Components::TagClass tag{Components::Tag::UI};
			ecs.addComponent(this->dim, timer);
			ecs.addComponent(this->dim, tag);
		}
		else
		{
			this->dim = ui_blank_solid_center(0, 0, AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), 0, this->z, 0.f, 0.f, 0.f, 0.f);
			Components::Timer timer{ 2.f, 0.f, true, false };
			Components::TagClass tag{ Components::Tag::UI };
			ecs.addComponent(this->dim, timer);
			ecs.addComponent(this->dim, tag);
		}
		
	}
	bool ScreenTransition::update()
	{
		if(this->dim != 0 && !this->finished){
			auto timer = ecs.getComponent<Components::Timer>(this->dim);
			auto color = ecs.getComponent<Components::Color>(this->dim);
			f32 lerp = timer->seconds/timer->max_seconds;

			if(timer->seconds >= timer->max_seconds)
			{
				this->finished = true;
				return false;
			}

			if(this->fadeIn)
			{
				color->d_color.a = 1.f - lerp;
			}
			else
			{
				color->d_color.a = lerp;
			}
			return true;
		}
		return false;
	}

	void ScreenTransition::free()
	{
		if(this->dim != 0) ecs.destroyEntity(this->dim);
		this->dim = 0;
	}


	Entity ui_hp_bar(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z)

	{

		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.5f, 1.0f, 0.5f ,1.0f };
		Components::HP hp{ 100 };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, hp);

		return id;
	}

	Entity ui_stamina_bar(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},{}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.0f, 0.8f, 1.0f ,1.0f };
		Components::TurnBasedStats stats{ 0,0,0, 100, 100, 100 };
		ecs.addComponent(id, stats);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	void button_onHover(Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.2f;

		c->d_color.r = minimum + (1.f - minimum) * lerp;
		c->d_color.b = minimum + (1.f - minimum) * lerp;
		c->d_color.g = minimum + (1.f - minimum) * lerp;

	}


	void button_offHover(Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		c->d_color = c->c_color;
		timer->seconds = 0.5f;

	}

	Entity ui_button(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::function<void()> func, Components::RGBA rgba)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ rgba.r, rgba.g, rgba.b, rgba.a };
		Components::Input in(AEVK_LBUTTON, true, func, [id] {button_onHover(id); }, [id] { button_offHover(id); }, z);	//add input system for grid
		Components::TagClass tag{ Components::Tag::UI };	//add input system for grid
		Components::Timer timer{ 1.f, 0.f, true, true };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, in);
		ecs.addComponent(id, tag);
		ecs.addComponent(id, timer);


		return id;
	}

	Entity ui_button_texture(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::function<void()> func)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Texture tex{ texture };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Input in(AEVK_LBUTTON, true, func, [id] { button_onHover(id); }, [id] { button_offHover(id); }, 10);	//add input system for grid
		Components::TagClass tag{ Components::Tag::UI };	//add input system for grid
		Components::Timer timer{ 1.f, 0.5f, true, true };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, tex);
		ecs.addComponent(id, color);
		ecs.addComponent(id, in);
		ecs.addComponent(id, tag);
		ecs.addComponent(id, timer);

		return id;
	}

	Entity ui_text(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, const char* a, Components::RGBA rgba)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Text text{ a, TF.getFontID(), z };
		Components::Color color{ rgba.r, rgba.g, rgba.b, rgba.a };
		Components::TagClass tag{ Components::Tag::UI };	
		ecs.addComponent(id, trans);
		ecs.addComponent(id, text);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tag);


		return id;
	}

	Entity ui_interactive_text(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, const char* a, Components::RGBA rgba)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Text text{ a, TF.getFontID(), z };
		Components::Color color{ rgba.r, rgba.g, rgba.b, rgba.a };
		Components::TagClass tag{ Components::Tag::UI };	//add input system for grid
		Components::Input in(AEVK_LBUTTON, true, nullptr, [id] { button_onHover(id); }, [id] { button_offHover(id); }, 10);	//add input system for grid
		Components::Timer timer{ 1.f, 0.5f, true, true };

		ecs.addComponent(id, trans);
		ecs.addComponent(id, text);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tag);
		ecs.addComponent(id, in);
		ecs.addComponent(id, timer);


		return id;
	}

	Entity ui_anchor(f32 x, f32 y, f32 width, f32 height, f32 rotation)
	{
		Entity id = ecs.createEntity();
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::TagClass tag{ Components::Tag::UI };
		ecs.addComponent(id, tag);
		ecs.addComponent(id, trans);


		return id;
	}

	Entity ui_blank_solid_corner(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 a)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ r, g, b, a };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	Entity ui_blank_solid_center(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 a)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ r, g, b, a };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	Entity ui_blank_texture(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture tex{ texture };
		Components::TagClass tag{ Components::Tag::UI };
		ecs.addComponent(id, tag);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tex);

		return id;
	}
	Entity ui_blank_texture_world(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture tex{ texture };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tex);

		return id;
	}


}