#pragma once

namespace UIO
{
	Entity ui_hp_bar(f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
	Entity ui_stamina_bar(f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);
	Entity ui_button(f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func);
	Entity ui_text(f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, const char* a);
	Entity ui_button_texture(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func);
	Entity ui_anchor(f32 x, f32 y, f32 width, f32 height, f32 rotation);
	Entity ui_blank_solid_corner(f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, f32 r, f32 g, f32 b, f32 a);
	Entity ui_blank_solid_center(f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, f32 r, f32 g, f32 b, f32 a);
	Entity ui_blank_texture(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z);

}