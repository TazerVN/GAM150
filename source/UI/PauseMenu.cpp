#include "pch.h"
#include "PauseMenu.h"
#include "../UI/UIObject.h"
#include "AEEngine.h"



void PauseMenu::init()
{

	if (!this->on && this->created && current_menu != CURRENT::EMPTY)
	{
		this->on = true;
		this->dim = UIO::ScreenTransition(true, 0.3f, 0.8f, 0.5f);
		Components::Input in{ AEVK_LBUTTON, true, nullptr, nullptr, nullptr, this->z - 1 };
		ecs.addComponent(dim.dim, in);
	}

	ecs.destroyEntity(this->timer);
	this->timer = UIO::ui_timer(1.f);

	switch (this->current_menu)
	{
		case CURRENT::MAIN:
		{
			pause = true;
			this->menu = new MainPause;
			this->menu->init();
			auto main = dynamic_cast<MainPause*>(this->menu);

			auto button_c = ecs.getComponent<Components::Input>(main->continue_button.button);
			button_c->onClick = [this]
			{
					if (!this->transition)
					{
						this->on = false;
						this->transition = true;
						this->menu->to_continue();
						
					}
			};

			auto button_s = ecs.getComponent<Components::Input>(main->setting_button.button);
			button_s->onClick = [this]
			{
					if (!this->transition)
					{
						this->on = false;
						this->transition = true;
						this->menu->to_setting();
					}
			};

			auto button_r = ecs.getComponent<Components::Input>(main->abandon_button.button);
			button_r->onClick = [this]
				{
					if (!this->transition)
					{
						this->on = false;
						this->transition = true;
						this->menu->to_confirm();
					
					}
				};

			auto button_l = ecs.getComponent<Components::Input>(main->leave_button.button);
			button_l->onClick = [this]
				{
					this->on = false;
					this->menu->to_exit();
					this->transition = true;
				};
			break;
		}
		case CURRENT::SETTING:
		{
			this->menu = new SettingPause;
			this->menu->init();
			auto setting = dynamic_cast<SettingPause*>(this->menu);

			auto button_l = ecs.getComponent<Components::Input>(setting->leave.button);
			button_l->onClick = [this]
				{
					this->menu->to_main();
					this->transition = true;
				};
			break;
		}
		case CURRENT::CONFIRM: break;
		case CURRENT::TUTORIAL: break;
		case CURRENT::EMPTY:
		{
			this->menu = nullptr;
		}
	}
}

//PauseMenu& PauseMenu::operator=(const PauseMenu& rhs)
//{
//	this->dim = rhs.dim;
//	this->continue_button = rhs.continue_button;
//	this->abandon_button = rhs.abandon_button;
//	this->leave_button = rhs.leave_button;
//	this->setting_button = rhs.setting_button;
//	this->created = true;
//
//	auto button_c = ecs.getComponent<Components::Input>(this->continue_button.button);
//	button_c->onClick = [this] { this->on = false; };
//
//	auto button_r = ecs.getComponent<Components::Input>(this->abandon_button.button);
//	button_r->onClick = [this] 
//	{
//		this->on = false;
//		player_died = true;
//	};
//
//	auto button_l = ecs.getComponent<Components::Input>(this->leave_button.button);
//	button_l->onClick = [this]
//	{
//		this->on = false;
//		gLevelStateNext = LevelStates::LS_QUIT;
//	};
//
//	return *this;
//}

void PauseMenu::update()
{
	if (this->menu == nullptr)
	{
		this->dim.update();
	}
	else
	{
		this->menu->update();

		switch (this->menu->dest)
		{
			case BasePauseMenu::DESTINATION::CONTINUE:
			{
				pause = false;
				this->current_menu = CURRENT::EMPTY;
				this->free();
				this->init();
				this->transition = false;
				break;
			}
			case BasePauseMenu::DESTINATION::MAIN:
			{
				this->current_menu = CURRENT::MAIN;
				this->menu->free();
				if (this->menu != nullptr)
				{
					this->menu->free();
					delete this->menu;
					this->menu = nullptr;
				}
				this->init();
				this->transition = false;
				break;
			}
			case BasePauseMenu::DESTINATION::CONFIRM: 
			{ 
				pause = false;
				this->current_menu = CURRENT::EMPTY;
				this->free();
				this->init();
				this->transition = false;
				break;
			}
			case BasePauseMenu::DESTINATION::SETTING:
			{ 
				this->current_menu = CURRENT::SETTING;
				this->menu->free();
				if (this->menu != nullptr)
				{
					this->menu->free();
					delete this->menu;
					this->menu = nullptr;
				}
				this->init();
				this->transition = false;
				break; 
			}
			case BasePauseMenu::DESTINATION::TUTORIAL: { break; }
			case BasePauseMenu::DESTINATION::EXIT:
			{ 
				pause = false;
				this->current_menu = CURRENT::EMPTY;
				this->free();
				this->init();
				this->transition = false;
				gLevelStateNext = LevelStates::LS_QUIT;
				this->dim = UIO::ScreenTransition(true, 1.f, 1.f, 0.5f);
				break;
			}

		}
	}
}

void PauseMenu::free()
{
	if (this->menu != nullptr)
	{
		this->menu->free();
		delete this->menu;
		this->menu = nullptr;
	}

	if (this->timer != 0)
	{
		ecs.destroyEntity(this->timer);
		this->timer = 0;
	}

	this->dim.free();
	this->on = false;
	this->created = false;
}
bool PauseMenu::isOn()
{
	return on;
}

bool PauseMenu::isCreated()
{
	return created;
}

void PauseMenu::setStateOn(bool flag)
{
	on = flag;
}
void PauseMenu::setStateCreate(bool flag)
{
	created = flag;
}

void BasePauseMenu::to_continue()
{
	this->dest = DESTINATION::CONTINUE;
}
void BasePauseMenu::to_main()
{
	this->dest = DESTINATION::MAIN;
}
void BasePauseMenu::to_confirm()
{
	this->dest = DESTINATION::CONFIRM;
}
void BasePauseMenu::to_tutorial()
{
	this->dest = DESTINATION::TUTORIAL;
}
void BasePauseMenu::to_setting()
{
	this->dest = DESTINATION::SETTING;
}
void BasePauseMenu::to_exit()
{
	this->dest = DESTINATION::EXIT;
}

void MainPause::free()
{
	this->continue_button.free();
	this->abandon_button.free();
	this->setting_button.free();
	this->tutorial_button.free();
	this->leave_button.free();
}
void MainPause::init()
{
	f32 size_x = 1.4f;
	f32 size_y = 1.8f;

	f32 start_y = 250.f;
	f32 start_x =/* AEGfxGetWinMinX() + 200.f * size_x*/ 0.f;
	f32 offset_y = 80.f * size_y;

	this->continue_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 0, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Continue", nullptr, { 1.f,1.f,1.f,1.f });

	this->abandon_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 1, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Abandon", nullptr, { 1.f,1.f,1.f,1.f });

	this->setting_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 2, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Setting", nullptr, { 1.f,1.f,1.f,1.f });

	this->tutorial_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 3, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Tutorial", nullptr, { 1.f,1.f,1.f,1.f });

	this->leave_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 4, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Exit", nullptr, { 1.f,1.f,1.f,1.f });
}

void SettingPause::init()
{
	f32 start_y = AEGfxGetWinMaxY() - 150.f;
	f32 start_x = AEGfxGetWinMinX() + 200.f;

	f32 size_x = 1.3f;
	f32 size_y = 1.5f;

	f32 s_h = 50.f * size_y;
	f32 s_w = 700.f * size_x;

	f32 offset_x = 256.f * size_x;
	f32 offset_y = 120.f * size_y;
	f32 title_s_x = 1.f;
	f32 title_s_y = 1.f;

	f32 text_size = 0.7f;

	f32 text_slider_gap = 300.f;




	this->bgm_text = UIO::TextShadow{ start_x, start_y - s_h * text_size, text_size, z , "Music" , 0xFFFFFFFF };
	this->bgm_slider = UIO::Slider{ start_x + text_slider_gap,  start_y - offset_y * 0, s_w, s_h, z, nullptr, AF.bgm.volume, 1.f };

	this->amb_text = UIO::TextShadow{ start_x,start_y - offset_y * 1 - s_h * text_size,  text_size, z , "Ambience" , 0xFFFFFFFF };
	this->amb_slider = UIO::Slider{ start_x + text_slider_gap,  start_y - offset_y * 1, s_w, s_h, z, nullptr, AF.amb.volume, 1.f };

	this->sfx_text = UIO::TextShadow{ start_x, start_y - offset_y * 2 - s_h * text_size, text_size, z , "SFX" , 0xFFFFFFFF };
	this->sfx_slider = UIO::Slider{ start_x + text_slider_gap ,  start_y - offset_y * 2, s_w, s_h, z, nullptr, AF.sfx.volume, 1.f };
	this->leave = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMaxX() - 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, 0.5f ,0.f, z, "GO BACK", nullptr, 0xFFFFFFFF };

}

void SettingPause::free()
{
	this->amb_slider.free();
	this->amb_text.free();

	this->bgm_slider.free();
	this->bgm_text.free();

	this->sfx_slider.free();
	this->sfx_text.free();

	this->leave.free();
}

void SettingPause::update()
{
	this->amb_slider.update();
	this->sfx_slider.update();
	this->bgm_slider.update();

	AF.bgm.setVolume(bgm_slider.current);
	AF.sfx.setVolume(sfx_slider.current);
	AF.amb.setVolume(amb_slider.current);
}

