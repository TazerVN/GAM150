#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "AEEngine.h"
#include "../util/util.h"
#include "../util/json_parser.h"

#include "EntityFactory.h"

namespace System {
	Entity create_actor_spritesheet(ECS::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, const char* name , f32 hp, AEGfxTexture* pTex)
	{
		Entity id = ecs.createEntity();
		//default player values
		//=====================Stats==========================
		Components::Name nm{ name };
		Components::Card_Storage card_storage;
		Components::HP HP{ hp };
		Components::TurnBasedStats tbstats
		{	5,	//max points
			0,	//cur_points
			0,	//shields
			7.f};	//movement spd
		//=====================Render==========================
		Components::Texture texture{pTex, 0.0f, 0.0f};
		Components::Transform trans{ pos,pos,size, size,0.f };
		//twan help me add more mesh
		Components::Mesh mesh{ true, mf.MeshGet(MESH_SPRITE), TEXTURE, MESH_RECTANGLE_CENTER, 1 };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };

		ecs.addComponent(id, nm);
		ecs.addComponent(id, HP);
		ecs.addComponent(id, card_storage);
		ecs.addComponent(id, tbstats);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);

		return id;
	}

	Entity create_actor_normal(ECS::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, const char* name, f32 hp, AEGfxTexture* pTex)
	{
		Entity id = ecs.createEntity();
		//default player values
		//=====================Stats==========================
		Components::Name nm{ name };
		Components::Card_Storage card_storage;
		Components::HP HP{ hp };
		Components::TurnBasedStats tbstats
		{ 5,	//max points
			0,	//cur_points
			0,	//shields
			7.f };	//movement spd
		//=====================Render==========================
		Components::Texture texture{ pTex, 0.0f, 0.0f };
		Components::Transform trans{ pos,pos,size, size,0.f };
		//twan help me add more mesh
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, 1 };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };

		ecs.addComponent(id, nm);
		ecs.addComponent(id, HP);
		ecs.addComponent(id, card_storage);
		ecs.addComponent(id, tbstats);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);

		return id;
	}

	Entity create_st_atk_card(ECS::Registry& ecs, const char* name, f32 atk,Components::DamageType dtype,f32 range,f32 cost)
	{
		Components::Targetting targetting_type = Components::Targetting::SINGLE_TARGET;
		Entity id = ecs.createEntity();
		//default player values
		Components::CardTag cardTag = Components::CardTag::ATTACK;
		Components::Name nm{ name };

		Components::Card_Value card_val{ atk,dtype };
		Components::Targetting_Component targetting{ targetting_type,range,0.f};

		Components::Card_Cost card_cost{ cost };
		ecs.addComponent(id, cardTag);
		ecs.addComponent(id, nm);
		ecs.addComponent(id, card_val);
		ecs.addComponent(id, targetting);
		ecs.addComponent(id, card_cost);
		return id;
	}

	Entity create_aoe_atk_card(ECS::Registry& ecs, const char* name, f32 atk, Components::DamageType dtype, f32 range, f32 aoe, f32 cost)
	{
		Components::Targetting targetting_type = Components::Targetting::AOE;
		Entity id = ecs.createEntity();
		//default player values
		Components::CardTag cardTag = Components::CardTag::ATTACK;
		Components::Name nm{ name };

		Components::Card_Value card_val{ atk,dtype };
		Components::Targetting_Component targetting{ targetting_type,range,aoe };

		Components::Card_Cost card_cost{ cost };
		ecs.addComponent(id, cardTag);
		ecs.addComponent(id, nm);
		ecs.addComponent(id, card_val);
		ecs.addComponent(id, targetting);
		ecs.addComponent(id, card_cost);
		return id;
	}

	void add_card_player_hand(ECS::Registry& ecs, Entity user, Entity cardID)
	{
		ECS::ComponentTypeID card_storage_ID = ECS::getComponentTypeID<Components::Card_Storage>();
		//if user dont have card storage return
		if (!(ecs.getBitMask()[user].test(card_storage_ID))) return;

		//get reference to the user's card_storage
		Components::Card_Storage* user_cards = ecs.getComponent<Components::Card_Storage>(user);

		user_cards->add_card_to_hand(cardID);
	}

	void add_card_player_deck(ECS::Registry& ecs, Entity user, Entity cardID)
	{
		ECS::ComponentTypeID card_storage_ID = ECS::getComponentTypeID<Components::Card_Storage>();
		//if user dont have card storage return
		if (!(ecs.getBitMask()[user].test(card_storage_ID))) return;

		//get reference to the user's card_storage
		Components::Card_Storage* user_cards = ecs.getComponent<Components::Card_Storage>(user);
		user_cards->add_card_to_deck(cardID);
	}

	void remove_card_player(ECS::Registry& ecs, Entity user, int index)
	{
		ECS::ComponentTypeID card_storage_ID = ECS::getComponentTypeID<Components::Card_Storage>();
		//if user dont have card storage return
		if (!(ecs.getBitMask()[user].test(card_storage_ID))) return;

		Components::Card_Storage* user_cards = ecs.getComponent<Components::Card_Storage>(user);
		//if the index is invalid return
		user_cards->remove_card_from_hand(index);
	}

	void CardSystem::init_cards(ECS::Registry& ecs) 
	{
		std::vector<JSON_CARD> vec;			
		JSON_RET ret = parse_data(vec, "../../Assets/cards/cards.json");

		cards.push_back(create_st_atk_card(ecs,"Sword Attack", 10.f, Components::DamageType::SLASHING,1.f,1.f));				//0
		cards.push_back(create_st_atk_card(ecs,"Fire Sword", 12.f, Components::DamageType::FIRE,2.f,1.f));						//1
		cards.push_back(create_st_atk_card(ecs,"Steven Sword", 20.f, Components::DamageType::PIERCING,3.f,1.f));				//2
		cards.push_back(create_st_atk_card(ecs,"Gun", 8.f, Components::DamageType::PIERCING, 9.f,1.f));							//3
		cards.push_back(create_aoe_atk_card(ecs, "Black Hole", 1.f, Components::DamageType::BLUDGEONING, 4.f, 3.f, 1.f));		//4
		//cards.push_back(create_defense_card(ecs,"Shield", 10.f, 0.f));							
	};
	Entity& CardSystem::get_card(int index) 
	{
		return cards[index];
	}
	size_t CardSystem::size() const
	{
		return cards.size();
	}
}