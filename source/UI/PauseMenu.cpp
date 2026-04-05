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
	this->transition = false;
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
					AF.sfx.play(1);
					if (!this->transition)
					{
						this->on = false;
						this->transition = true;
						this->menu->to_continue();

					}
					AF.sfx.stopping = false;
				};

			auto button_s = ecs.getComponent<Components::Input>(main->setting_button.button);
			button_s->onClick = [this]
				{
					AF.sfx.play(1);
					if (!this->transition)
					{
						this->transition = true;
						this->menu->to_setting();
					}
					AF.sfx.stopping = false;
				};

			auto button_r = ecs.getComponent<Components::Input>(main->abandon_button.button);
			button_r->onClick = [this]
				{
					AF.sfx.play(1);
					if (!this->transition)
					{
						this->transition = true;
						this->menu->to_confirm();

					}
					AF.sfx.stopping = false;
				};

			auto button_t = ecs.getComponent<Components::Input>(main->tutorial_button.button);
			button_t->onClick = [this]
				{
					AF.sfx.play(1);
					if (!this->transition)
					{
						this->transition = true;
						this->menu->to_tutorial();

					}
					AF.sfx.stopping = false;
				};

			auto button_l = ecs.getComponent<Components::Input>(main->leave_button.button);
			button_l->onClick = [this]
				{
					AF.sfx.play(1);
					if (!this->transition)
					{
						this->on = false;
						this->transition = true;
						this->menu->to_exit();
					}
					AF.sfx.stopping = false;
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
					if (!this->transition)
					{
						AF.sfx.play(1);
						this->transition = true;
						this->menu->to_main();
						AF.sfx.stopping = false;
					}
				};
			break;
		}
		case CURRENT::CONFIRM:
		{
			this->menu = new ConfirmPause;
			this->menu->init();
			auto confirm = dynamic_cast<ConfirmPause*>(this->menu);

			auto button_l = ecs.getComponent<Components::Input>(confirm->no.button);
			button_l->onClick = [this]
				{
					AF.sfx.play(1);
					if (!this->transition)
					{
						this->transition = true;
						this->menu->to_main();
					}
					AF.sfx.stopping = false;
				};
			auto button_c = ecs.getComponent<Components::Input>(confirm->yes.button);
			button_c->onClick = [this]
				{
					AF.sfx.play(1);
					if (!this->transition)
					{
						this->on = false;
						this->transition = true;
						this->menu->to_gameover();
					}
					AF.sfx.stopping = false;
				};
			break;
		}
		case CURRENT::TUTORIAL:
		{
			this->menu = new TutorialPause;
			this->menu->init();
			auto tutorial = dynamic_cast<TutorialPause*>(this->menu);

			auto button_l = ecs.getComponent<Components::Input>(tutorial->leave_button.button);
			button_l->onClick = [this]
				{
					AF.sfx.play(1);
					if (!this->transition)
					{
						this->transition = true;
						this->menu->to_main();
					}
					AF.sfx.stopping = false;
				};
			break;

		};

		case CURRENT::EMPTY:
		{
			this->menu = nullptr;
		}
	}
}


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
				this->transition = false;
				break;
			}
			case BasePauseMenu::DESTINATION::MAIN:
			{
				this->current_menu = CURRENT::MAIN;
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
				this->current_menu = CURRENT::CONFIRM;
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
			case BasePauseMenu::DESTINATION::SETTING:
			{
				this->current_menu = CURRENT::SETTING;
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
			case BasePauseMenu::DESTINATION::TUTORIAL:
			{
				this->current_menu = CURRENT::TUTORIAL;
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

			case BasePauseMenu::DESTINATION::EXIT:
			{
				pause = false;
				this->current_menu = CURRENT::EMPTY;
				this->free();
				this->transition = false;
				gLevelStateNext = LevelStates::LS_QUIT;
				this->dim = UIO::ScreenTransition(true, 1.f, 1.f, 0.5f);
				break;
			}
			case BasePauseMenu::DESTINATION::GAMEOVER:
			{
				pause = false;
				this->current_menu = CURRENT::EMPTY;
				this->free();
				this->transition = false;
				
				player_died = true;
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
void BasePauseMenu::to_gameover()
{
	this->dest = DESTINATION::GAMEOVER;
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

	f32 start_y = 300.f;
	f32 start_x =/* AEGfxGetWinMinX() + 200.f * size_x*/ 0.f;
	f32 offset_y = 80.f * size_y;

	this->continue_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 0, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Continue", nullptr, { 1.f,1.f,1.f,1.f });

	this->abandon_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 1, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Abandon", nullptr, { 1.f,1.f,1.f,1.f });

	this->setting_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 2, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Setting", nullptr, { 1.f,1.f,1.f,1.f });

	this->tutorial_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 3, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Control", nullptr, { 1.f,1.f,1.f,1.f });

	this->leave_button = UIO::TextureButton(TF.getTextureUI(11), start_x, start_y - offset_y * 4, 256.f * size_x, 61.f * size_y, 0.5f, 0, z + 1, "Exit", nullptr, { 1.f,1.f,1.f,1.f });
}

void SettingPause::init()
{
	f32 start_y = AEGfxGetWinMaxY() - 150.f;
	f32 start_x = AEGfxGetWinMinX() + 200.f;

	f32 size_x = 1.4f;
	f32 size_y = 1.8f;

	f32 s_h = 50.f * size_y;
	f32 s_w = 700.f * size_x;

	f32 offset_x = 256.f * size_x;
	f32 offset_y = 120.f * size_y;

	f32 text_size = 0.7f;

	f32 text_slider_gap = 300.f;




	this->bgm_text = UIO::TextShadow{ start_x, start_y - s_h * text_size, text_size, z , "Music" , 0xFFFFFFFF };
	this->bgm_slider = UIO::Slider{ start_x + text_slider_gap,  start_y - offset_y * 0, s_w, s_h, z, nullptr, AF.bgm.volume, 1.f };

	this->amb_text = UIO::TextShadow{ start_x,start_y - offset_y * 1 - s_h * text_size,  text_size, z , "Ambience" , 0xFFFFFFFF };
	this->amb_slider = UIO::Slider{ start_x + text_slider_gap,  start_y - offset_y * 1, s_w, s_h, z, nullptr, AF.amb.volume, 1.f };

	this->sfx_text = UIO::TextShadow{ start_x, start_y - offset_y * 2 - s_h * text_size, text_size, z , "SFX" , 0xFFFFFFFF };
	this->sfx_slider = UIO::Slider{ start_x + text_slider_gap ,  start_y - offset_y * 2, s_w, s_h, z, nullptr, AF.sfx.volume, 1.f };
	this->leave = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMaxX() - 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, 0.5f ,0.f, z + 1, "GO BACK", nullptr, 0xFFFFFFFF };

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

void TutorialPause::init()
{

	f32 size_x = 1.3f;
	f32 size_y = 1.7f;

	f32 s_h = 50.f * size_y;
	f32 s_w = 700.f * size_x;

	f32 offset_x = 256.f * size_x;
	f32 offset_y = 120.f * size_y;

	f32 text_size = 0.5f;

	f32 text_slider_gap = 300.f;

	f32 image_w = 0.8f;
	f32 image_h = 0.8f;

	auto foward = [&]
		{
			AF.sfx.play(1);
			this->page += 1;
			AF.sfx.stopping = false;
		};
	auto backward = [&]
		{
			AF.sfx.play(1);
			this->page -= 1;
			if (this->page < 0) this->page = 10;
			AF.sfx.stopping = false;
		};

	this->page = 0;
	this->image = UIO::ui_blank_texture(TF.getTextureTutorial(0), 0, 0, AEGfxGetWindowWidth() * image_w, AEGfxGetWindowHeight() * image_h, 0, this->z + 1);

	this->leave_button = UIO::TextureButton{ TF.getTextureUI(11) , 0 , AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, text_size ,0.f, this->z + 1, "Leave", nullptr, 0xFFFFFFFF };
	this->foward_button = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMaxX() - 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, text_size ,0.f, this->z + 1, "Continue", foward, 0xFFFFFFFF };
	this->backward_button = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, text_size ,0.f, this->z + 1, "Back", backward, 0xFFFFFFFF };

}

void TutorialPause::update()
{
	this->page = this->page % 11;
	auto image = ecs.getComponent<Components::Texture>(this->image);
	if (image != nullptr)
	{
		image->texture = TF.getTextureTutorial(s8(this->page));
	}
}

void TutorialPause::free()
{
	if (this->image != 0)
	{
		ecs.destroyEntity(this->image);
		this->image = 0;
	}
	this->leave_button.free();
	this->foward_button.free();
	this->backward_button.free();
}

void ConfirmPause::free()
{
	yes.free();
	no.free();
	warning.free();
	description.free();
	question.free();
}

void ConfirmPause::init()
{
	f32 size_x = 1.4f;
	f32 size_y = 1.8f;


	f32 text_size = 0.5f;
	f32 text_size_warning = 0.8f;

	const char* warning_text = "Warning!!";
	const char* description_text = "The following action will permanently delete your existing run";
	//const char* description_text2 = "existing run";
	const char* question_text = "Do you want to proceed?";

	f32 offset_y = 64.f * 2.f;

	f32 warning_w, warning_h;
	f32 description_w, description_h;
	f32 description2_w, description2_h;
	f32 question_w, question_h;

	f32 win_w = AEGfxGetWindowWidth() * 0.25f;

	f32 win_h = (f32)AEGfxGetWindowHeight();
	AEGfxGetPrintSize(TF.getFontID(), warning_text, text_size_warning, &warning_w, &warning_h);
	AEGfxGetPrintSize(TF.getFontID(), description_text, text_size, &description_w, &description_h);
	//AEGfxGetPrintSize(TF.getFontID(), description_text2, text_size, &description2_w, &description2_h );
	AEGfxGetPrintSize(TF.getFontID(), question_text, text_size, &question_w, &question_h);

	f32 start_y = AEGfxGetWinMaxY() - win_h * 0.1f;

	this->warning = UIO::TextShadow{ -warning_w * win_w, start_y - offset_y, text_size_warning, this->z, warning_text, {1.f, 0.f, 0.f, 1.f} };
	this->description = UIO::TextShadow{ -description_w * win_w , start_y - offset_y * 2, text_size, this->z, description_text, {1.f, 1.f, 1.f, 1.f} };
	this->question = UIO::TextShadow{ -question_w * win_w, start_y - offset_y * 4, text_size, this->z, question_text, {1.f, 0.f, 0.f, 1.f} };

	this->no = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, text_size ,0.f, this->z, "No", nullptr, 0xFFFFFFFF };
	this->yes = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMaxX() - 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, text_size ,0.f, this->z, "Yes", nullptr, 0xFFFFFFFF };
}



