#include "../ECS/ECSystem.h"
#include "../rendering/Mesh_factory.h"
#include "AEEngine.h"

#include "Systems.h"

namespace System {
	void Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target)
	{
		//attack component
		ECS::ComponentTypeID atkID = ECS::getComponentTypeID<Components::Attack>();
		//test if card have attack id
		bool yes = ecs.getBitMask()[cardID].test(atkID);
		if (!yes)
		{
			return;
		}

		ECS::ComponentTypeID hpID = ECS::getComponentTypeID<Components::HP>();
		if (!(ecs.getBitMask()[target].test(hpID)))
		{
			return;
		}

		//if the have components then reduce the HP amount
		ecs.getComponent<Components::HP>(target)->value -= ecs.getComponent<Components::Attack>(cardID)->damage;

	}

	Entity create_player(ECS::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, std::string name , f32 hp)
	{
		Entity id = ecs.createEntity();
		//default player values
		//=====================Stats==========================
		Components::Name nm{ name };
		Components::Card_Storage card_storage;
		Components::HP HP{ hp };
		Components::TurnBasedStats tbs;
		//=====================Render==========================
		Components::Transform trans{ pos,pos,size,0.f };
		//twan help me add more mesh
		Components::Mesh mesh{ mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, 1 };
		Components::Color color{ { 1.0f, 1.0f, 1.0f ,1.0f }, { 1.0f, 1.0f, 1.0f ,1.0f } };

		ecs.addComponent(id, nm);
		ecs.addComponent(id, HP);
		ecs.addComponent(id, card_storage);
		ecs.addComponent(id, tbs);

		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	Entity create_atk_card(ECS::Registry& ecs, f32 x, f32 y, std::string name, f32 atk,Components::DamageType dtype)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Name nm{ name };
		Components::Transform trans{ {x,y},{x,y},{10,10}, 0.0f };
		Components::Card_Storage card_storage;
		Components::Attack attack{ atk, dtype};
		ecs.addComponent(id, attack);
		ecs.addComponent(id, nm);
		ecs.addComponent(id, card_storage);

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
		size_t index = user_cards->get_nextIndex();
		user_cards->card_storage[index] = cardID;
	}

	void CardSystem::init_cards(ECS::Registry& ecs) 
	{
		cards.push_back(create_atk_card(ecs, 0.f, 0.f, "Sword Attack", 30.f, Components::SLASHING));	//0
		cards.push_back(create_atk_card(ecs, 0.f, 0.f, "Fire Sword", 40.f, Components::FIRE));			//1
	};
	Entity& CardSystem::get_card(int index) 
	{
		return cards[index];
	}
}