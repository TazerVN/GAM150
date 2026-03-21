#include "pch.h"
#include "EntityFactory.h"
#include <functional>

namespace EntityFactory {
	Entity create_actor_spritesheet(AEVec2 pos, AEVec2 size, const char* name , f32 hp, AEGfxTexture* pTex, Components::AnimationType at)
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
		Components::Timer timer{ 1.f, 0.5f, true, true };
		Components::Animation_Actor aa{ at, 0, 6};
		Components::AStarResult as{};
		Components::Tag tag{ Components::Tag::ACTOR };

		ecs.addComponent(id, nm);
		ecs.addComponent(id, HP);
		ecs.addComponent(id, card_storage);
		ecs.addComponent(id, tbstats);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);
		ecs.addComponent(id, timer);
		ecs.addComponent(id, aa);
		ecs.addComponent(id, as);
		ecs.addComponent(id, tag);

		return id;
	}

	Entity create_actor_normal(EntityComponent::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, const char* name, f32 hp, AEGfxTexture* pTex, Components::AnimationType at)
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
			7.f };	//movement spd
		//=====================Render==========================
		Components::Texture texture{ pTex, 0.0f, 0.0f };
		Components::Transform trans{ pos,pos,size, size,0.f };
		//twan help me add more mesh
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, 1 };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Timer timer{1.f, 0.5f, true, true};
		Components::Animation_Actor aa{at};
		Components::Tag tag{ Components::Tag::ACTOR };

		ecs.addComponent(id, nm);
		ecs.addComponent(id, HP);
		ecs.addComponent(id, card_storage);
		ecs.addComponent(id, tbstats);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, texture);
		ecs.addComponent(id, timer);
		ecs.addComponent(id, aa);
		ecs.addComponent(id, tag);

		return id;
	}

	void InteractableNode::init(Grid::GameBoard& gb)
	{
		gbptr = &gb;
	}

	Entity InteractableNode::create_interactable_node(EntityComponent::Registry& ecs, MeshFactory& mf, AEVec2 pos, AEVec2 size, AEGfxTexture* pTex
		, Components::AnimationType at, Components::VictoryNodeTag vic)
	{
		Entity id = ecs.createEntity();
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, 1 };
		Components::Texture texture{ pTex, 0.0f, 0.0f };
		Components::Transform trans{ pos,pos,size, size,0.f };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Tag tag {Components::Tag::OTHERS};

		ecs.addComponent(id, mesh);
		ecs.addComponent(id, tag);
		ecs.addComponent(id, texture);
		ecs.addComponent(id, color);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, at);
		ecs.addComponent(id, vic);

		this->nodes.push_back(id);
		return id;
	}

	void InteractableNode::free()
	{
		for (Entity node : nodes)
		{
			ecs.destroyEntity(node);
		}
		nodes.clear();
	}

	void InteractableNode::update()
	{
		s32& x = gbptr->cur_x; s32& y = gbptr->cur_y;
		//guard
		if (x >= MAX_I || x < 0 || y > MAX_J || y < 0) return;

		for (Entity node : nodes)
		{
			if (gbptr->get_pos()[x][y] == node)
			{
				switch (*(ecs.getComponent<Components::VictoryNodeTag>(node)))
				{
				case Components::VictoryNodeTag::COMBAT:
				{
					gLevelStateNext = LevelStates::LS_COMBAT;
					break;
				}
				case Components::VictoryNodeTag::BOSS:
				{
					gLevelStateNext = LevelStates::LS_BOSS;
					break;
				}
				default:
					break;
				}
			}
		}
	}

	void goToCombat()
	{

	}
	void goToBoss()
	{

	}

	void add_card_player_hand(EntityComponent::Registry& ecs, Entity user, Entity cardID)
	{
		EntityComponent::ComponentTypeID card_storage_ID = EntityComponent::getComponentTypeID<Components::Card_Storage>();
		//if user dont have card storage return
		if (!(ecs.getBitMask()[user].test(card_storage_ID))) return;

		//get reference to the user's card_storage
		Components::Card_Storage* user_cards = ecs.getComponent<Components::Card_Storage>(user);

		user_cards->add_card_to_hand(cardID);
	}

	void add_card_player_deck(EntityComponent::Registry& ecs, Entity user, Entity cardID)
	{
		EntityComponent::ComponentTypeID card_storage_ID = EntityComponent::getComponentTypeID<Components::Card_Storage>();
		//if user dont have card storage return
		if (!(ecs.getBitMask()[user].test(card_storage_ID))) return;

		//get reference to the user's card_storage
		Components::Card_Storage* user_cards = ecs.getComponent<Components::Card_Storage>(user);
		user_cards->add_card_to_deck(cardID);
	}

	void remove_card_player(EntityComponent::Registry& ecs, Entity user, int index)
	{
		EntityComponent::ComponentTypeID card_storage_ID = EntityComponent::getComponentTypeID<Components::Card_Storage>();
		//if user dont have card storage return
		if (!(ecs.getBitMask()[user].test(card_storage_ID))) return;

		Components::Card_Storage* user_cards = ecs.getComponent<Components::Card_Storage>(user);
		//if the index is invalid return
		user_cards->remove_card_from_hand(index);
	}
}