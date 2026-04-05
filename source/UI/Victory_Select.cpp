#include "pch.h"
#include "Victory_Select.h"
#include "UIObject.h"


VictoryCard::VictoryCard(Victory_Select* vs, Entity cardID, f32 x, f32 y)
{
	auto cost = ecs.getComponent<Components::Card_Cost>(cardID);

	this->card = vs->create_Victory_card(cardID, x, y);
	auto transform = ecs.getComponent<Components::Transform>(this->card);

	this->mana = UIO::ui_interactive_text(x - transform->size.x * 0.40f, y + transform->size.y * 0.37f, 0.7f, 0.f, 0, vs->z + 1, std::to_string(static_cast<int>(cost->value)).c_str(), { 1.f,1.f,1.f,1.f });


}
VictoryCard::VictoryCard(Victory_Select* vs, std::string cardname, f32 x, f32 y)
{
	Entity _card = card_system.get_bible_id(cardname);
	auto cost = ecs.getComponent<Components::Card_Cost>(_card);
	this->card = vs->create_Victory_card(cardname, x, y);

	auto transform = ecs.getComponent<Components::Transform>(this->card);
	
	this->mana = UIO::ui_interactive_text(x - transform->size.x * 0.40f, y + transform->size.y * 0.37f,  0.7f, 0.f, 0, vs->z + 1, std::to_string(static_cast<int>(cost->value)).c_str(), { 1.f,1.f,1.f,1.f });
}


void VictoryCard::free()
{
	if (this->card != 0)
	{
		ecs.destroyEntity(this->card);
		this->card = 0;
	}
	if (this->mana != 0)
	{
		ecs.destroyEntity(this->mana);
		this->mana = 0;
	}
}

void Victory_Select::update()
{
	if (this->on)
	{
		this->on = false;

		this->dim = UIO::ScreenTransition(false, 0.f ,0.7f, 1.f);

		this->dim.z = 1150;

		auto* mesh = ecs.getComponent<Components::Mesh>(this->dim.dim);
		if (mesh)
		{
			mesh->z = 1150;
		}

		Components::Input in{ AEVK_LBUTTON, true, nullptr, nullptr, nullptr, 40 };
		ecs.addComponent(dim.dim, in);

		for(int i = 0; i < 3; i++)
		{
			f32 x = i/3.f * AEGfxGetWindowWidth() + AEGfxGetWinMinX() + 324.f * 0.8f;

			int upper_bound = static_cast<int>(card_system.size()) - 1;
			int lower_bound = 0;
			int index = std::rand() % (upper_bound - lower_bound + 1) + lower_bound;
			Entity cardBibleID = card_system.getCardAtIndex(index);
			VictoryCard vc{ this, static_cast<Entity>(cardBibleID), x, 0.f };
			victory_cards.push_back(vc);
		}

		f32 size_x = 1.4f;
		f32 size_y = 1.8f;
		f32 text_size = 0.5f;

		auto skp = [&](){ this->free();};
		this->skip = UIO::TextureButton{ TF.getTextureUI(11) , 0 , AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, text_size ,0.f, this->z + 1, "Skip", skp, 0xFFFFFFFF };
	}
	this->dim.update();
	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		this->free();
	}
}
void Victory_Select::free()
{
	for (VictoryCard vc : victory_cards)
	{
		vc.free();
	}
	dim.free();
	victory_cards.clear();
	skip.free();
}



Entity Victory_Select::create_Victory_card(std::string cardname, f32 x, f32 y)
{
	Entity _card = card_system.get_bible_id(cardname);

	//============texture==================
	std::string& loc = ecs.getComponent<Components::image_location>(_card)->location;
	AEGfxTexture* pTex = TF.getTextureFromCardMap(loc);

	//===========Victory Card================

	auto click = [cardname, this]
		{
			Entity actual_card = card_system.generate_card_from_bible(cardname);
			ecs.getComponent<Components::Card_Storage>(playerID)->add_card_to_deck(actual_card);
			this->free();
		};
	Entity vc_id = UIO::ui_button_texture(pTex, x, y, 324 * 0.8f, 528 * 0.8f, 0, this->z + 1, click);

	//======================================

	return vc_id;
}

Entity Victory_Select::create_Victory_card(Entity cardID, f32 x, f32 y)
{
	//============texture==================
	std::string& loc = ecs.getComponent<Components::image_location>(cardID)->location;
	AEGfxTexture* pTex = TF.getTextureFromCardMap(loc);

	//===========Victory Card================

	auto click = [cardID, this]
		{
			Entity actual_card = card_system.generate_card_from_bible(cardID);
			ecs.getComponent<Components::Card_Storage>(playerID)->add_card_to_deck(actual_card);
			this->free();
		};

	Entity vc_id = UIO::ui_button_texture(pTex, x, y, 324 * 0.8f, 528 * 0.8f, 0, this->z + 1, click);
	//======================================

	return vc_id;
}
