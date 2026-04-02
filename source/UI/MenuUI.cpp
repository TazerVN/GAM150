#include "pch.h"
#include "MenuUI.h"

BaseMenu::BaseMenu() : dest{ DESTINATION::NONE }
{
}

MenuUI::MenuUI()
{
	//this->play = UIO::TextButton{ 0.f,  -50.f, 300.f, 100.f, 0.6f, 0.f, 1200, "PLAY", [] { gGameStateNext = GameStates::GS_Game; }, 0xFF12CCFF };
	//this->option = UIO::TextButton{ 0.f,  -200.f, 300.f, 100.f, 0.6f, 0.f, 1200, "PLAY", nullptr, 0xFF12CCFF };
	//this->exit=  UIO::TextButton{ 0.f, -350.f, 300.f, 100.f, 0.6f ,0.f, 1200, "EXIT", nullptr, 0xFFCC66FF };

	//this->title = UIO::TextShadow(-0.72f * AEGfxGetWinMaxX(), 0.2f * AEGfxGetWinMaxY(), -0.72f * AEGfxGetWinMaxX(), 0.25f * AEGfxGetWinMaxY(), 2.0f, 1200, "Beyond The Nexus", 0x000000FF);


	////title
	//UIO::ui_text(-0.72f * AEGfxGetWinMaxX(), 0.2f * AEGfxGetWinMaxY(), 2.0f, 2.0f, 0, 1200, "Beyond The Nexus", 0x000000FF);
	//UIO::ui_text(-0.72f * AEGfxGetWinMaxX(), 0.25f * AEGfxGetWinMaxY(), 2.0f, 2.0f, 0, 1200, "Beyond The Nexus", 0xFFFFFFFF);
}


void MainMenu::free()
{
	this->play.free();
	this->setting.free();
	this->exit.free();
	this->credit.free();
	ecs.destroyEntity(this->title);
}

void BaseMenu::to_game()
{
	this->dest = DESTINATION::GAME;
}

void BaseMenu::to_setting()
{
	this->dest = DESTINATION::SETTING;
}

void BaseMenu::to_credit()
{
	this->dest = DESTINATION::CREDIT;

}

void BaseMenu::to_main()
{
	this->dest = DESTINATION::MAIN;
}

void MainMenu::init()
{
	f32 start_y = -300.f;
	f32 start_x = 300.f;

	f32 size_x = 1.3f;
	f32 size_y = 1.5f;


	f32 offset_x = 256.f * size_x;
	f32 offset_y = 0.f;
	f32 title_s_x = 1.f;
	f32 title_s_y = 1.f;

	f32 text_size = 0.5f;


	this->play = UIO::TextureButton{ TF.getTextureUI(11) ,AEGfxGetWinMinX() + start_x,  start_y, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "PLAY", nullptr, 0xFFFFFFFF };
	this->setting = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + start_x + offset_x, start_y + offset_y, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "SETTING", nullptr, 0xFFFFFFFF };
	this->credit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + start_x + offset_x * 2, start_y + offset_y * 2, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "CREDIT", nullptr, 0xFFFFFFFF };
	this->exit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + start_x + offset_x * 3, start_y + offset_y * 3,256.f * size_x, 61.f * size_y, text_size ,0.f, 1200, "EXIT", nullptr, 0xFFFFFFFF };


	this->title = UIO::ui_blank_texture(TF.getTextureOthers(9), 0, 100.f, 957 * title_s_x, 513 * title_s_y, 0.f, 1200);


}

MainMenu::MainMenu() : BaseMenu()
{

}


void MenuUI::free()
{
	switch (this->cur)
	{
		case (MenuUI::CURRENT_MENU::MAIN):
		{
			this->main.free();
			break;
		}
		case (MenuUI::CURRENT_MENU::SETTING):
		{
			this->setting.free();
			break;
		}
		case (MenuUI::CURRENT_MENU::CREDIT):
		{
			this->credit.free();
			break;
		}
	}

	this->fade.free();
}


void MenuUI::init()
{
	this->fade.free();
	this->fade = UIO::ScreenTransition(true);
	this->transition = true;

	switch (this->cur)
	{
		case (MenuUI::CURRENT_MENU::MAIN):
		{
			this->main.dest = BaseMenu::DESTINATION::NONE;
			this->main.init();
			auto play = ecs.getComponent<Components::Input>(this->main.play.button);
			play->onClick = [this]
				{
					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false, 1.f };
						this->main.to_game();
						this->transition = true;
					}
				};
			auto setting = ecs.getComponent<Components::Input>(this->main.setting.button);
			setting->onClick = [this]
				{
					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false, 1.f };
						this->main.to_setting();
						this->transition = true;
					}
				};
			auto credit = ecs.getComponent<Components::Input>(this->main.credit.button);
			credit->onClick = [this]
				{
					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false, 1.f };
						this->main.to_credit();
						this->transition = true;
					}
				};
			auto exit = ecs.getComponent<Components::Input>(this->main.exit.button);
			exit->onClick = [this]
				{
					if (!this->transition)
					{
						gGameStateNext = GameStates::GS_QUIT;
					}
				};
			break;
		}
		case (MenuUI::CURRENT_MENU::SETTING):
		{
			this->setting.init();
			this->setting.dest = BaseMenu::DESTINATION::NONE;

			auto exit = ecs.getComponent<Components::Input>(this->setting.exit.button);
			exit->onClick = [this]()
				{

					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false, 1.f };
						this->setting.to_main();
						this->transition = true;
					}
				};
			break;
		}
		case (MenuUI::CURRENT_MENU::CREDIT):
		{

			this->credit.init();
			this->credit.dest = BaseMenu::DESTINATION::NONE;

			auto exit = ecs.getComponent<Components::Input>(this->credit.exit.button);
			exit->onClick = [this]()
				{

					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false, 1.f };
						this->credit.to_main();
						this->transition = true;
					}
				};
			break;
		}
	}


}

void MenuUI::update()
{
	switch (this->cur)
	{
		case (MenuUI::CURRENT_MENU::MAIN):
		{
			switch (this->main.dest)
			{

				case(BaseMenu::DESTINATION::GAME):
				{
					if (!new_Start)
					{
						//draw pop up
					}
					else
					{
						auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
						if (!this->fade.update())
						{
							gGameStateNext = GameStates::GS_Game;
							this->cur = CURRENT_MENU::MAIN;
							this->main.dest = BaseMenu::DESTINATION::NONE;
							this->transition = false;

						}
					}
					break;
				}

				case(BaseMenu::DESTINATION::SETTING):
				{
					auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
					if (!this->fade.update())
					{
						this->free();
						this->cur = CURRENT_MENU::SETTING;
						this->main.dest = BaseMenu::DESTINATION::NONE;
						this->init();
						this->transition = false;

					}
					break;
				}
				case(BaseMenu::DESTINATION::CREDIT):
				{
					auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
					if (!this->fade.update())
					{
						this->free();
						this->cur = CURRENT_MENU::CREDIT;
						this->main.dest = BaseMenu::DESTINATION::NONE;
						this->init();
						this->transition = false;

					}
					break;
				}
				default:
					if (!this->fade.update())
					{
						this->transition = false;
					};
					break;

			}
			break;
		case (MenuUI::CURRENT_MENU::SETTING):
		{
			switch (this->setting.dest)
			{
				case(BaseMenu::DESTINATION::MAIN):
				{
					auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
					if (!this->fade.update())
					{
						this->free();
						this->cur = CURRENT_MENU::MAIN;
						this->setting.dest = BaseMenu::DESTINATION::NONE;
						this->init();
						this->transition = false;
					}
					break;
				}


				default:
				{
					if (!this->fade.update())
					{
						this->transition = false;
					};
					break;
				}

			}
			//this->setting.update();
			break;
		}
		case (MenuUI::CURRENT_MENU::CREDIT):
		{
			switch (this->credit.dest)
			{
				case(BaseMenu::DESTINATION::MAIN):
				{
					auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
					if (!this->fade.update())
					{
						this->free();
						this->cur = CURRENT_MENU::MAIN;
						this->credit.dest = BaseMenu::DESTINATION::NONE;
						this->init();
						this->transition = false;
					}
					break;
				}


				default:
				{
					if (!this->fade.update())
					{
						this->transition = false;
					};
					break;
				}

				break;
			}
		}
		}
	}
}

void SettingMenu::init()
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


	this->bgm_text = UIO::TextShadow{ start_x, start_y - s_h * text_size, text_size, 1200 , "Music" , 0xFFFFFFFF };
	this->bgm_slider = UIO::Slider{ start_x + text_slider_gap,  start_y - offset_y * 0, s_w, s_h, 1200, nullptr };

	this->amb_text = UIO::TextShadow{ start_x,start_y - offset_y * 1 - s_h * text_size,  text_size, 1200 , "Ambience" , 0xFFFFFFFF };
	this->amb_slider = UIO::Slider{ start_x + text_slider_gap,  start_y - offset_y * 1, s_w, s_h, 1200, nullptr };

	this->sfx_text = UIO::TextShadow{ start_x, start_y - offset_y * 2 - s_h * text_size, text_size, 1200 , "SFX" , 0xFFFFFFFF };
	this->sfx_slider = UIO::Slider{ start_x + text_slider_gap ,  start_y - offset_y * 2, s_w, s_h, 1200, nullptr };
	this->exit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMaxX() - 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, 0.5f ,0.f, 1200, "GO BACK", nullptr, 0xFFFFFFFF };


}


void SettingMenu::free()
{
	this->amb_slider.free();
	this->amb_text.free();

	this->bgm_slider.free();
	this->bgm_text.free();

	this->sfx_slider.free();
	this->sfx_text.free();

	this->exit.free();
}



void CreditMenu::init()
{
	f32 size_x = 1.3f;
	f32 size_y = 1.5f;

	this->exit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMaxX() - 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, 0.5f ,0.f, 1200, "GO BACK", nullptr, 0xFFFFFFFF };

}
void CreditMenu::free()
{
	exit.free();
}

