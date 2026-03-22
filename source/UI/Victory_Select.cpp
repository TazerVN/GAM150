#include "pch.h"
#include "Victory_Select.h"
#include "UIObject.h"

void Victory_Select::update()
{
	if (this->on)
	{
		this->on = false;

		this->dim = UIO::ui_blank_solid_center(ecs, mf, 0, 0, AEGfxGetWindowWidth() * 1.5f, AEGfxGetWindowHeight() * 1.5f, 0, 40, 0.0f, 0.0f, 0.0f, 0.5f);
		Components::TagClass tag{ Components::Tag::UI };
		Components::Input in{ AEVK_LBUTTON, true, nullptr, nullptr, nullptr, 40 };
		ecs.addComponent(dim, tag);
		ecs.addComponent(dim, in);

		for(int i = 0; i < 3; i++)
		{
			f32 x = i/3.f * AEGfxGetWindowWidth() + AEGfxGetWinMinX() + 324.f * 0.8f;
			victory_cards.push_back(create_Victory_card("Slash", x, 0.f));
		}
	}


}
void Victory_Select::free()
{
	for (Entity vc : victory_cards)
	{
		ecs.destroyEntity(vc);
	}
	ecs.destroyEntity(dim);
	victory_cards.clear();
}

Entity Victory_Select::create_Victory_card(std::string cardname, f32 x, f32 y)
{
	Entity _card = card_system.get_bible_id(cardname);

	//============texture==================
	std::string& loc = ecs.getComponent<Components::image_location>(_card)->location;
	AEGfxTexture* pTex = TF.getTextureFromCardMap(loc);

	//===========Victory Card================
	Entity vc_id = ecs.createEntity();

	Components::Transform trans{ {x,y},{x,y}, {324 * 0.8f, 528 * 0.8f,}, {324 * 0.8f, 528 * 0.8f}, 0.0f };
	Components::Input input(AEVK_LBUTTON, true,
		[cardname,this]
		{
			Entity actual_card = card_system.generate_card_from_bible(cardname);
			ecs.getComponent<Components::Card_Storage>(playerID)->add_card_to_deck(actual_card);
			this->free();
		},
		nullptr, nullptr, 40);
	Components::Texture texture{ pTex };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, 41 };
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