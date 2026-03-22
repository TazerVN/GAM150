#include "pch.h"
#include "cardInformation.h"
#include <iostream>

namespace CardInformation
{
	CardDisplay::CardDisplay(const CardDisplay& rhs)
	{
		this->on = rhs.on;
		this->created = rhs.on;
		this->info_id = rhs.info_id;
	}

	CardDisplay& CardDisplay::operator=(const CardDisplay& rhs)
	{
		this->on = rhs.on;
		this->created = rhs.on;
		this->info_id = rhs.info_id;

		if (this->current_card_id != 0)
		{
			this->text = Info(ecs, this->info_id, current_card_id);
		}
		return *this;
	}

	CardDisplay::CardDisplay(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, s32 z) : text{}, current_card_id{}, previous_card_id{}
	{
		Entity id = ecs.createEntity();
		this->info_id = id;
		on = true;
		created = false;

		Components::Transform trans{ {x,y}, {x,y} ,{width, height}, {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, s8(z) };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture texture{ TF.getTextureUI(7) };
		Components::TagClass tag{ Components::Tag::UI };


		ecs.addComponent(id, tag);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);


	}

	bool CardDisplay::isOn()
	{
		return on;
	}
	bool CardDisplay::isCreated()
	{
		return created;
	}
	void CardDisplay::setStateOn(bool flag)
	{
		this->on = flag;
	}
	void CardDisplay::setStateCreated(bool flag)
	{
		this->created = flag;
	}

	void CardDisplay::setCurrentCard(Entity card_data)
	{
		this->previous_card_id = this->current_card_id;
		this->current_card_id = card_data;
	}


	void CardDisplay::free(EntityComponent::Registry& ecs)
	{
		created = false;
		if (this->info_id != 0)
		{
			ecs.destroyEntity(this->info_id);
			this->info_id = 0;
		}

		text.free();

	}
	void CardDisplay::update()
	{
		if(this->current_card_id != 0 && this->created && this->on)
		{
			auto card_name = ecs.getComponent<Components::Name>(this->current_card_id);
			auto card_cost = ecs.getComponent<Components::Card_Cost>(this->current_card_id);

			auto card_name_cur = ecs.getComponent<Components::Text>(this->text.name);
			card_name_cur->text = card_name->value;

			auto card_cost_cur = ecs.getComponent<Components::Text>(this->text.cost);

			card_cost_cur->text = std::to_string(static_cast<int>(card_cost->value));
		}
	
	}

	CardDisplay::Info::Info(EntityComponent::Registry& ecs, Entity display ,Entity CardData)
	{

		auto display_transform = ecs.getComponent<Components::Transform>(display);

		auto card_name = ecs.getComponent<Components::Name>(CardData);
		auto card_cost = ecs.getComponent<Components::Card_Cost>(CardData);

		s8 z = 31;

		f32 x = display_transform->pos.x;
		f32 y = display_transform->pos.y;

		

		//name entity
		this->name = ecs.createEntity();
		//default player values
		Components::Transform trans_name{ {x - 50.f,y + 150.f}, {x - 50.f,y + 150.f} ,{0.5f, 0.f} , {0.5f, 0},0.0f };
		Components::Text text_name{ card_name->value, TF.getFontID(), z};
		Components::Color color_name{ 0.8f, 1.0f, 1.0f ,1.0f };
		Components::TagClass tag_name{ Components::Tag::UI };	//add input system for grid
		ecs.addComponent(this->name, trans_name);
		ecs.addComponent(this->name, text_name);
		ecs.addComponent(this->name, color_name);
		ecs.addComponent(this->name, tag_name);

		this->cost = ecs.createEntity();
		Components::Transform trans_cost{ {x - 155.f,y + 155.f}, {x - 155.f,y + 155.f} ,{0.7f, 0.f} , {0.7f, 0},0.0f };
		Components::Text text_cost{ std::to_string(static_cast<int>(card_cost->value)), TF.getFontID(), z};
		Components::Color color_cost{ 0.8f, 1.0f, 1.0f ,1.0f };
		Components::TagClass tag_cost{ Components::Tag::UI };	//add input system for grid
		ecs.addComponent(this->cost, trans_cost);
		ecs.addComponent(this->cost, text_cost);
		ecs.addComponent(this->cost, color_cost);
		ecs.addComponent(this->cost, tag_cost);

		this->description = ecs.createEntity();
		Components::Transform trans_des{ {x - 100.f,y}, {x - 100.f,y} ,{0.3f, 0.f} , {0.3f, 0},0.0f };
		Components::Text text_des{ "no description man?", TF.getFontID(), z};
		Components::Color color_des{ 0.8f, 1.0f, 1.0f ,1.0f };
		Components::TagClass tag_des{ Components::Tag::UI };	//add input system for grid
		ecs.addComponent(this->description, trans_des);
		ecs.addComponent(this->description, text_des);
		ecs.addComponent(this->description, color_des);
		ecs.addComponent(this->description, tag_des);


	}

	void CardDisplay::Info::free()
	{
		if (this->cost != 0)
		{
			ecs.destroyEntity(this->cost);
			this->cost = 0;
		}

		if (this->description != 0)
		{
			ecs.destroyEntity(this->description);
			this->description = 0;
		}
		if (this->name != 0)
		{
			ecs.destroyEntity(this->name);
			this->name = 0;
		}
	}
}