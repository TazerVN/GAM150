#include "pch.h"

#include "Components.h"

#include <iostream>

namespace Components
{

	void Components::Card_Storage::add_card_to_deck(Entity cardID)
	{
		data_deck.push_back(cardID);
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
			data_deck.push_back(card);
		}
		data_discard_pile.clear();
	}

	Components::Input::Input(u8 type, bool hover,
							 std::function<void()> onClick,
							 std::function<void()> onHover,
							 std::function<void()> offHover, s8 z, bool drag, bool col)
		: type(type), hover(hover), onClick(onClick),
		onHover(onHover), offHover(offHover), on(true), z{ z }, drag{ drag }, col{ col }
	{
	}

	Components::Text::Text(const char* text, s8 fontID, s8 z) : text{ text }, fontID{ fontID }, z{ z } {}

	Components::RGBA::RGBA(f32 r, f32 g, f32 b, f32 a) : r{ r }, g{ g }, b{ b }, a{ a } {};
	Components::Color::Color(f32 r, f32 g, f32 b, f32 a) : c_color{ r,g,b,a }, d_color{ r,g,b,a } {}

	Components::Timer::Timer() : seconds{ 0 }, max_seconds{ 0 }, start{ false }, reset{ false } {}
	Components::Timer::Timer(f32 max, f32 current, bool start, bool reset) : seconds{ current }, max_seconds{ max }, start{ start }, reset{ reset } {}

	Components::HP::HP(f32 max) : c_value{ max }, max_value{ max } {}

	Components::Texture::Texture(AEGfxTexture* texture, f32 offset_x, f32 offset_y) : texture{ texture }, offset_x{ offset_x }, offset_y{ offset_y } {}

	Animation_Actor::Animation_Actor(Components::AnimationType type, int current, int max)
		: anim_type{ type }, finished{ false }, timer_array{ size_t(-1) }, current_frame{ current }, max_frame{ max }

	{
	}

}