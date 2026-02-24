#include "Components.h"


Components::Input::Input(u8 type, bool hover,
						 std::function<void()> onClick,
						 std::function<void()> onHover,
						 std::function<void()> offHover)
	: type(type), hover(hover), onClick(onClick),
	onHover(onHover), offHover(offHover), on(true)
{
}

Components::Text::Text(const char* text, s8& fontID, s8 z) : text{ text }, fontID{fontID}, z{z}{}

Components::RGBA::RGBA(f32 r, f32 g, f32 b, f32 a) : r{ r }, g{ g }, b{ b }, a{a} {};
Components::Color::Color(f32 r, f32 g, f32 b, f32 a) : c_color{ r,g,b,a }, p_color{r,g,b,a} {}