#include "pch.h"

#include "Components.h"
#include <utility>
#include <iostream>

void Components::Card_Storage::add_card_to_deck(Entity cardID)
{
	original_draw_pile.push_back(cardID);
}

void Components::Card_Storage::add_card_to_hand(Entity cardID)
{
	data_card_hand.push_back(cardID);
}

void Components::Card_Storage::remove_card_from_hand(int index)
{
	data_discard_pile.push_back(data_card_hand[index]);
	data_card_hand.erase(data_card_hand.begin() + index);
}

void Components::Card_Storage::reshuffle_discard2deck()
{
	if (data_discard_pile.empty()) { std::cout << "Discard Pile Empty" << std::endl; return; }
	for (Entity card : data_discard_pile)
	{
		data_draw_pile.push_back(card);
	}
	data_discard_pile.clear();
}

void Components::Card_Storage::init()
{
	data_draw_pile = original_draw_pile;
}

void Components::Card_Storage::free()
{
	data_draw_pile.clear();
	data_card_hand.clear();
	data_discard_pile.clear();
}

void Components::Card_Storage::reset()
{
	for (Entity card : original_draw_pile)
	{
		ecs.destroyEntity(card);
	}
	original_draw_pile.clear();
}

Components::Input::Input(u8 type, bool hover,
	std::function<void()> onClick,
	std::function<void()> onHover,
	std::function<void()> offHover, s32 z, bool drag, bool col, 
						 std::function<void()> onDrag, 
						 std::function<void()> offDrag)
	: type(type), hover(hover), onClick(onClick),
	onHover(onHover), offHover(offHover), on(true), z{ z }, drag{ drag }, col{ col }, onDrag{onDrag}, offDrag{offDrag}
{
}

Components::Text::Text(std::string text, s8 fontID, s32 z) : text{ text }, fontID{ fontID }, z{ z } {}



Components::RGBA::RGBA(f32 r, f32 g, f32 b, f32 a) : r{ r }, g{ g }, b{ b }, a{ a } {};
Components::Color::Color(f32 r, f32 g, f32 b, f32 a) : c_color{ r,g,b,a }, d_color{ r,g,b,a } {}

Components::Timer::Timer() : seconds{ 0 }, max_seconds{ 0 }, start{ false }, reset{ false } {}
Components::Timer::Timer(f32 max, f32 current, bool start, bool reset) : seconds{ current }, max_seconds{ max }, start{ start }, reset{ reset } {}

Components::HP::HP(f32 max) : c_value{ max }, max_value{ max } {}

Components::Texture::Texture(AEGfxTexture* texture, f32 offset_x, f32 offset_y) : texture{ texture }, offset_x{ offset_x }, offset_y{ offset_y } {}

Components::Animation_Actor::Animation_Actor(Components::AnimationType type, int current, int max)
	: prev_type{type} ,anim_type{ type }, default_type{type}, finished {false}, timer_array{ size_t(-1) }, current_frame{ current }, max_frame{ max }

{
}

void Components::Animation_Actor::setType(Components::AnimationType at)
{
	this->prev_type = this->anim_type;
	this->anim_type = at;
}

Components::AnimationType Components::Animation_Actor::getCurrType()
{
	return this->anim_type;
}

Components::AnimationType Components::Animation_Actor::getDefaultType()
{
	return this->default_type;
}
void Components::Horde_Tag::remove_goon(Entity removed_goon)
{
	for (size_t itr = 0;  itr < goons.size(); ++itr)
	{
		if (goons[itr] == removed_goon)
		{
			std::swap(goons[itr], goons.back());
			goons.pop_back();
		}
	}
}

bool Components::Horde_Tag::alive() const
{
	return !(goons.empty());
}