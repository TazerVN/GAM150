#include "Components.h"
#include "../ECS/ECSystem.h"
#include <iostream>

void Components::Card_Storage::add_card_to_hand(Entity cardID)
{
	data_card_hand.push_back(cardID);
}

void Components::Card_Storage::remove_card_from_hand(int index)
{
	data_card_hand.erase(data_card_hand.begin() + index);
}


Components::Input::Input(u8 type, bool hover,
						 std::function<void()> onClick,
						 std::function<void()> onHover,
						 std::function<void()> offHover)
	: type(type), hover(hover), onClick(onClick),
	onHover(onHover), offHover(offHover), on(true)
{
}

Components::Text::Text(const char* text, s8 fontID, s8 z) : text{ text }, fontID{fontID}, z{z}{}

Components::RGBA::RGBA(f32 r, f32 g, f32 b, f32 a) : r{ r }, g{ g }, b{ b }, a{a} {};
Components::Color::Color(f32 r, f32 g, f32 b, f32 a) : c_color{ r,g,b,a }, p_color{r,g,b,a} {}

Components::Timer::Timer() : seconds{ 0 }, max_seconds{ 0 }, start{ false }, reset{ false } {}
Components::Timer::Timer(f32 max, f32 current, bool start, bool reset) : seconds{ current }, max_seconds{ max }, start{ start }, reset{reset} {}