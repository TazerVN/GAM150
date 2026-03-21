#include "pch.h"
#include "UIObject.h"

namespace UIO
{

	Entity ui_hp_bar(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)

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

	Entity ui_stamina_bar(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},{}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.0f, 0.8f, 1.0f ,1.0f };
		Components::TurnBasedStats stats{ 0,0,0, 100 };
		ecs.addComponent(id, stats);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	void button_onHover(EntityComponent::Registry& ecs, Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.2f;

		c->d_color.r = minimum + (1.f - minimum) * lerp;
		c->d_color.b = minimum + (1.f - minimum) * lerp;
		c->d_color.g = minimum + (1.f - minimum) * lerp;

	}


	void button_offHover(EntityComponent::Registry& ecs, Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		c->d_color = c->c_color;
		timer->seconds = 0.5f;

	}

	Entity ui_button(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 0.5, 0.5f, 0.5f ,1.0f };
		Components::Input in(AEVK_LBUTTON, true, func, [&ecs, id] { button_onHover(ecs, id); }, [&ecs, id] { button_offHover(ecs, id); }, 10);	//add input system for grid
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

	Entity ui_button_texture(EntityComponent::Registry& ecs, MeshFactory& mf, AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Texture tex{ texture };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Input in(AEVK_LBUTTON, true, func, [&ecs, id] { button_onHover(ecs, id); }, [&ecs, id] { button_offHover(ecs, id); }, 10);	//add input system for grid
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

	Entity ui_text(EntityComponent::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, const char* a)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Text text{ a, tf.getFontID(), z };
		Components::Color color{ 0.8f, 1.0f, 1.0f ,1.0f };
		Components::TagClass tag{ Components::Tag::UI };	//add input system for grid
		ecs.addComponent(id, trans);
		ecs.addComponent(id, text);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tag);


		return id;
	}

	Entity ui_anchor(EntityComponent::Registry& ecs, f32 x, f32 y, f32 width, f32 height, f32 rotation)
	{
		Entity id = ecs.createEntity();
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::TagClass tag{ Components::Tag::UI };
		ecs.addComponent(id, tag);
		ecs.addComponent(id, trans);


		return id;
	}

	Entity ui_blank(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, f32 r, f32 g, f32 b, f32 a)
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

	Entity ui_blank_texture(EntityComponent::Registry& ecs, MeshFactory& mf, AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
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
}