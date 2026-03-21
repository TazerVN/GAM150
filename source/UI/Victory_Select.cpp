#include "pch.h"
#include "Victory_Select.h"

void Victory_Select::update()
{
	if (this->on)
	{
		this->on = false;
		victory_cards.push_back(create_Victory_card("Slash", 0.f, 0.f));
	}
}
void Victory_Select::free()
{
	for (Entity vc : victory_cards)
	{
		ecs.destroyEntity(vc);
	}
	victory_cards.clear();
}

Entity Victory_Select::create_Victory_card(std::string cardname, f32 x, f32 y)
{
	Entity _card = card_system.get_bible_id(cardname);

	//============texture==================
	std::string& loc = ecs.getComponent<Components::card_image>(_card)->location;
	AEGfxTexture* pTex = TF.getTextureFromCardMap(loc);

	//===========Victory Card================
	Entity vc_id = ecs.createEntity();

	Components::Transform trans{ {x,y},{x,y}, {324, 528}, {324, 528}, 0.0f };
	Components::Input input(AEVK_LBUTTON, true,
		[cardname,this]
		{
			Entity actual_card = card_system.generate_card_from_bible(cardname);
			ecs.getComponent<Components::Card_Storage>(playerID)->add_card_to_deck(actual_card);
			this->free();
		},
		nullptr, nullptr, 10);
	Components::Texture texture{ pTex };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, 30 };
	Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
	Components::TagClass tag { Components::Tag::UI };

	ecs.addComponent(vc_id, mesh);
	ecs.addComponent(vc_id, trans);
	ecs.addComponent(vc_id, input);
	ecs.addComponent(vc_id, texture);
	ecs.addComponent(vc_id, color);
	ecs.addComponent(vc_id, tag);
	//======================================

	return vc_id;
}