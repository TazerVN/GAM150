//=========================================
//	AUTHOR:		PHAM MINH TUAN
// 
//	EMAIL:		minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================
#pragma once
#include "types.h"
#include "AEEngine.h"
#include "ECS/Components.h"
#include <functional>

namespace UIO
{
	struct TextShadow
	{
		s32 z{ 1200 };
		Entity text{ 0 };
		Entity text_shadow{ 0 };
		TextShadow() = default;
		TextShadow(f32 x, f32 y, f32 text_size, s32 z, std::string a, Components::RGBA rgba);
		bool newline() const;
		TextShadow& operator=(TextShadow const& );
		TextShadow& operator+=(const char * rhs);

		void free();
	};


	
	struct TextButton
	{
		public:
		s32 z{0};
		bool on{false};
		Entity timer{ 0 };
		Entity button{ 0 };
		Entity text{ 0 };
		TextButton() = default;
		TextButton(f32 x, f32 y, f32 width, f32 height, f32 text_size, f32 rotation, s32 z, std::string a, std::function<void()> func, Components::RGBA rgba);

		void free();

	};

	struct TextureButton : public TextButton
	{
		public:
		TextureButton(const TextureButton&);
		TextureButton& operator=(const TextureButton&);
		TextureButton() = default;
		TextureButton(AEGfxTexture* texture ,f32 x, f32 y, f32 width, f32 height, f32 text_size, f32 rotation, s32 z, std::string a, std::function<void()> func, Components::RGBA rgba);
		void setOpacity(f32 a);
		void free();

	};

	

	struct ScreenTransition
	{
		public:
		s32 z{1300};
		Entity dim{0};
		bool fadeIn{false};
		bool finished{false};
		f32 min{ 0.f };
		f32 max{ 1.f };
		ScreenTransition() = default;
		ScreenTransition(bool fadeIn, f32 min = 0.f,  f32 max = 1.f, f32 duration = 0.5f);
		ScreenTransition& operator=(ScreenTransition const&);
		bool update();
		void free();
	};

	struct Slider{
		f32 current{};
		f32 max{};
		Entity blank{};
		Entity fill{};
		Entity button{};
		Slider() = default;
		Slider(f32 x, f32 y, f32 width, f32 height, s32 z, f32 current, f32 max);
		void update();
		void free();
	};


	Entity ui_timer(f32 max, f32 min = 0.f);
	Entity ui_hp_bar(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z);
	Entity ui_stamina_bar(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z);
	Entity ui_button(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::function<void()> func, Components::RGBA rgba = { 0.1f, 0.3f, 0.5f ,1.0f });
	Entity ui_text(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::string a, Components::RGBA rgba = {0.8f, 1.0f, 1.0f, 1.0f});
	Entity ui_interactive_text(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::string a, Components::RGBA rgba = {0.8f, 1.0f, 1.0f, 1.0f});
	Entity ui_button_texture(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::function<void()> func);
	Entity ui_anchor(f32 x, f32 y, f32 width, f32 height, f32 rotation);
	Entity ui_blank_solid_corner(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 a);
	Entity ui_blank_solid_center(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 a);
	Entity ui_blank_texture(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z);
	Entity ui_blank_texture_world(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z);

}