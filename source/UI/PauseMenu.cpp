#include "pch.h"
#include "PauseMenu.h"
#include "../UI/UIObject.h"
#include "AEEngine.h"



void PauseMenu::init()
{

	if (!this->on && this->created && current_menu != CURRENT::EMPTY)
	{
		this->on = true;
		this->dim = UIO::ScreenTransition(true, 0.3f, 0.5f, 0.5f);
		Components::Input in{ AEVK_LBUTTON, true, nullptr, nullptr, nullptr, this->z };
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
						ecs.destroyEntity(this->timer);
						this->timer = UIO::ui_timer(1.f);
					}
				};

			auto button_r = ecs.getComponent<Components::Input>(main->abandon_button.button);
			button_r->onClick = [this]
				{
					this->on = false;
					player_died = true;
				};

			auto button_l = ecs.getComponent<Components::Input>(main->leave_button.button);
			button_l->onClick = [this]
				{
					this->on = false;
					gLevelStateNext = LevelStates::LS_QUIT;
				};
			break;
		}
		case CURRENT::CONFIRM: break;
		case CURRENT::SETTING: break;
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
				this->dim.free();
				this->init();
				this->transition = false;
				break;
			}
			case BasePauseMenu::DESTINATION::CONFIRM: { break; }
			case BasePauseMenu::DESTINATION::SETTING: { break; }
			case BasePauseMenu::DESTINATION::TUTORIAL: { break; }
			case BasePauseMenu::DESTINATION::EXIT: { break; }

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



