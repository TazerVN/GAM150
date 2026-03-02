#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "AEEngine.h"
#include "../util/util.h"

#include "EntityFactory.h"

namespace System {
	//void Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target)
	//{
	//	//attack component
	//	ECS::ComponentTypeID atkID = ECS::getComponentTypeID<Components::Attack>();
	//	//test if card have attack id
	//	if (!ecs.getBitMask()[cardID].test(atkID)) return;

	//	ECS::ComponentTypeID hpID = ECS::getComponentTypeID<Components::HP>();
	//	if (!(ecs.getBitMask()[target].test(hpID))) return;

	//	//if the have components then reduce the HP amount
	//	ecs.getComponent<Components::HP>(target)->value -= ecs.getComponent<Components::Attack>(cardID)->damage;
	//	
	//}

	Entity create_player(ECS::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, const char* name , f32 hp, AEGfxTexture* pTex)
	{
		Entity id = ecs.createEntity();
		//default player values
		//=====================Stats==========================
		Components::Name nm{ name };
		Components::Card_Storage card_storage;
		Components::HP HP{ hp };
		Components::TurnBasedStats tbs{5, 0, false, 0.f};
		//=====================Render==========================
		Components::Texture texture{pTex};
		Components::Transform trans{ pos,pos,size, size,0.f };
		//twan help me add more mesh
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, 1 };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };

		ecs.addComponent(id, nm);
		ecs.addComponent(id, HP);
		ecs.addComponent(id, card_storage);
		ecs.addComponent(id, tbs);

		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);

		return id;
	}

	Entity create_atk_card(ECS::Registry& ecs, const char* name, f32 atk,Components::DamageType dtype,f32 range)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::CardTag cardTag = Components::CardTag::ATTACK;
		Components::Name nm{ name };
		Components::Attack attack{ atk, dtype, range};
		ecs.addComponent(id, cardTag);
		ecs.addComponent(id, nm);
		ecs.addComponent(id, attack);
		return id;
	}

	Entity create_defense_card(ECS::Registry& ecs, const char* name, f32 val, f32 range)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::CardTag cardTag{ Components::CardTag::DEFENSE };
		Components::Name nm{ name };
		Components::Defense def {val,range};
		ecs.addComponent(id, cardTag);
		ecs.addComponent(id, nm);
		return id;
	}

	void add_card_player(ECS::Registry& ecs, Entity user, Entity cardID)
	{
		ECS::ComponentTypeID card_storage_ID = ECS::getComponentTypeID<Components::Card_Storage>();
		//if user dont have card storage return
		if (!(ecs.getBitMask()[user].test(card_storage_ID))) return;

		//std::array<Entity, MAX_HAND>& user_storage = ecs.getComponent<Components::Card_Storage>(user)->card_storage;
		//get reference to the user's card_storage
		Components::Card_Storage* user_cards = ecs.getComponent<Components::Card_Storage>(user);
		user_cards->add_card_to_hand(cardID);
	}

	void remove_card_player(ECS::Registry& ecs, Entity user, int index)
	{
		ECS::ComponentTypeID card_storage_ID = ECS::getComponentTypeID<Components::Card_Storage>();
		//if user dont have card storage return
		if (!(ecs.getBitMask()[user].test(card_storage_ID))) return;

		if (index >= MAX_HAND || index < 0) return;

		Components::Card_Storage* user_cards = ecs.getComponent<Components::Card_Storage>(user);
		//if the index is invalid return
		user_cards->remove_card_from_hand(index);
	}

	void CardSystem::init_cards(ECS::Registry& ecs) 
	{
		cards.push_back(create_atk_card(ecs,"Sword Attack", 5.f, Components::SLASHING,1.f));	//0
		cards.push_back(create_atk_card(ecs,"Fire Sword", 4.f, Components::FIRE,2.f));			//1
		cards.push_back(create_atk_card(ecs,"Steven Sword", 10.f, Components::PIERCING,3.f));	//2
		cards.push_back(create_atk_card(ecs,"Gun", 2.f, Components::PIERCING, 4.f));			//3
		//cards.push_back(create_defense_card(ecs,"Shield", 10.f, 0.f));							//4
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