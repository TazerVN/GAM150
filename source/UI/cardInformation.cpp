#include "pch.h"
#include "cardInformation.h"
#include <iostream>
#include <string>
#include <sstream>

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
		this->z = rhs.z;

		if (this->current_card_id != 0)
		{
			this->text = Info(ecs, this->info_id, current_card_id, this->z + 1);
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
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
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
		if (this->current_card_id != 0 && this->created && this->on)
		{
			auto card_name = ecs.getComponent<Components::Name>(this->current_card_id);
			auto card_cost = ecs.getComponent<Components::Card_Cost>(this->current_card_id);

			auto card_name_cur = ecs.getComponent<Components::Text>(this->text.name);
			card_name_cur->text = card_name->value;

			auto card_cost_cur = ecs.getComponent<Components::Text>(this->text.cost);

			card_cost_cur->text = std::to_string(static_cast<int>(card_cost->value));



			auto card_des = ecs.getComponent<Components::Card_Description>(this->current_card_id);
			auto des_trans = ecs.getComponent<Components::Transform>(this->text.description[0]);

			auto count = std::count(card_des->val.begin(), card_des->val.end(), '\n');


			des_trans->size.x = card_des->val.size() <= 30 ? des_trans->size_og.x * 1.2f : des_trans->size_og.x;

			std::string token;
			std::stringstream ss{ card_des->val };
			for (int i = 0; i < 7; i++)
			{
				if(i < count) std::getline(ss, token, '\n');
				else{
					token = "";
				}
				auto card_des_cur = ecs.getComponent<Components::Text>(this->text.description[i]);
				card_des_cur->text = token;
			}

		}

	}

	CardDisplay::Info::Info(EntityComponent::Registry& ecs, Entity display, Entity CardData, s32 z)
	{

		auto display_transform = ecs.getComponent<Components::Transform>(display);

		auto card_name = ecs.getComponent<Components::Name>(CardData);
		auto card_cost = ecs.getComponent<Components::Card_Cost>(CardData);
		auto card_des = ecs.getComponent<Components::Card_Description>(CardData);

		this->z = z;

		f32 x = display_transform->pos.x;
		f32 y = display_transform->pos.y;



		//name entity
		this->name = ecs.createEntity();
		//default player values
		Components::Transform trans_name{ {x - 50.f,y + 200.f}, {x - 50.f,y + 200.f} ,{0.5f, 0.f} , {0.5f, 0},0.0f };
		Components::Text text_name{ card_name->value, TF.getFontID(), z };
		Components::Color color_name{ 0.8f, 1.0f, 1.0f ,1.0f };
		Components::TagClass tag_name{ Components::Tag::UI };	//add input system for grid
		ecs.addComponent(this->name, trans_name);
		ecs.addComponent(this->name, text_name);
		ecs.addComponent(this->name, color_name);
		ecs.addComponent(this->name, tag_name);

		//cost
		this->cost = ecs.createEntity();
		Components::Transform trans_cost{ {x - 200.f,y + 200.f}, {x - 200.f,y + 200.f} ,{0.8f, 0.f} , {0.8f, 0},0.0f };
		Components::Text text_cost{ std::to_string(static_cast<int>(card_cost->value)), TF.getFontID(), z };
		Components::Color color_cost{ 0.8f, 1.0f, 1.0f ,1.0f };
		Components::TagClass tag_cost{ Components::Tag::UI };	//add input system for grid
		ecs.addComponent(this->cost, trans_cost);
		ecs.addComponent(this->cost, text_cost);
		ecs.addComponent(this->cost, color_cost);
		ecs.addComponent(this->cost, tag_cost);

		//description


		f32 size = 0.3f;
		std::string token;
		std::stringstream ss{ card_des->val };

		for (int i = 0; i < 7; i++)
		{
			std::getline(ss, token, '\n');

			Entity des = ecs.createEntity();
			this->description.push_back(des);
			Components::Transform trans_des{ {x - 160.f, y + 100.f - 50.f * i }, {x - 160.f, y + 100.f - 50.f * i} ,{size, size} , {size, size},0.0f };
			Components::Text text_des{ token, TF.getFontID(), z };
			Components::Color color_des{ 0.8f, 1.0f, 1.0f ,1.0f };
			Components::TagClass tag_des{ Components::Tag::UI };	//add input system for grid
			ecs.addComponent(des, trans_des);
			ecs.addComponent(des, text_des);
			ecs.addComponent(des, color_des);
			ecs.addComponent(des, tag_des);

		}


	}

	void CardDisplay::Info::free()
	{
		if (this->cost != 0)
		{
			ecs.destroyEntity(this->cost);
			this->cost = 0;
		}

		for (Entity ent : this->description)
		{
			ecs.destroyEntity(ent);
		}
		this->description.clear();

		if (this->name != 0)
		{
			ecs.destroyEntity(this->name);
			this->name = 0;
		}
	}
}