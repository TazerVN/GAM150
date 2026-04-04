#include "pch.h"
#include "MenuUI.h"
#include <vector>


BaseMenu::BaseMenu() : dest{ DESTINATION::NONE } {}



void BaseMenu::to_game()
{
	this->dest = DESTINATION::GAME;
}

void BaseMenu::to_setting()
{
	this->dest = DESTINATION::SETTING;
}

void BaseMenu::to_play()
{
	this->dest = DESTINATION::PLAY;
}

void BaseMenu::to_credit()
{
	this->dest = DESTINATION::CREDIT;

}

void BaseMenu::to_main()
{
	this->dest = DESTINATION::MAIN;
}

void BaseMenu::to_confirm()
{
	this->dest = DESTINATION::CONFIRM;
}
void BaseMenu::to_tutorial()
{
	this->dest = DESTINATION::TUTORIAL;
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

void GameOverUI::init()
{


	auto game = [this]()
		{
			if (!this->transition)
			{
				this->fade.free();
				this->fade = UIO::ScreenTransition{ false};
				this->transition = true;
				this->dest = NEXT::GAME;
			}
		};

	auto menu = [this]()
		{
			if (!this->transition)
			{
				this->fade.free();
				this->fade = UIO::ScreenTransition{ false};
				this->transition = true;
				this->dest = NEXT::MENU;
			}
		};

	f32 start_y = -300.f;
	f32 start_x = 450.f;

	f32 size_x = 1.3f;
	f32 size_y = 1.5f;


	f32 offset_x = 256.f * size_x * 2.f;
	f32 offset_y = 0.f;
	f32 title_s_x = 1.f;
	f32 title_s_y = 1.f;

	f32 text_size = 0.5f;

	this->transition = true;
	this->fade = UIO::ScreenTransition(true);
	this->dest = NEXT::NONE;

	this->background = UIO::ui_blank_texture(TF.getTextureOthers(10), 0, 0, AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), 0, 1100);

	this->new_game = UIO::TextureButton{ TF.getTextureUI(11) ,AEGfxGetWinMinX() + start_x,  start_y, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "New Game", game, 0xFFFFFFFF };
	this->exit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + start_x + offset_x, start_y + offset_y, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "Go Back", menu, 0xFFFFFFFF };


}

void GameOverUI::update()
{

	switch (this->dest)
	{
		case NEXT::NONE:
		{
			if (!this->fade.update())
			{
				this->transition = false;
			}
			break;
		}
		case NEXT::MENU:
		{
			if (!this->fade.update())
			{
				gGameStateNext = GameStates::GS_MAINMENU;
				this->transition = false;
			}
			break;
		}
		case NEXT::GAME:
		{
			if (!this->fade.update())
			{
				gGameStateNext = GameStates::GS_Game;
				this->transition = false;
			}
			break;
		}

	}
	//auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
	//if (!this->fade.update())
	//{
	//	this->free()
	//	gGameStateNext = GameStates::GS_Game;
	//	//this->current_menu->dest = BaseMenu::DESTINATION::NONE;
	//	this->transition = false;
	//}
}

void GameOverUI::free()
{
	this->fade.free();
	if (this->background != 0)
	{
		ecs.destroyEntity(this->background);
		this->background = 0;
	}
	this->new_game.free();
	this->exit.free();
}


void MenuUI::init()
{

	if (this->current_menu != nullptr)
	{
		this->current_menu->free();
		delete this->current_menu;
		this->current_menu = nullptr;
	}

	this->fade.free();
	this->fade = UIO::ScreenTransition(true);
	this->transition = true;

	if (background == 0)
	{
		background = UIO::ui_blank_texture(TF.getTextureOthers(8), 0, 0, AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), 0, 1100);
	}

	switch (this->cur)
	{
		case (MenuUI::CURRENT_MENU::MAIN):
		{
			this->current_menu = new MainMenu;
			this->current_menu->dest = BaseMenu::DESTINATION::NONE;
			this->current_menu->init();

			auto main = dynamic_cast<MainMenu*>(this->current_menu);

			auto play = ecs.getComponent<Components::Input>(main->play.button);
			play->onClick = [this]
				{
					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false };
						this->current_menu->to_play();
						this->transition = true;
					}
				};
			auto setting = ecs.getComponent<Components::Input>(main->setting.button);
			setting->onClick = [this]
				{
					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false};
						this->current_menu->to_setting();
						this->transition = true;
					}
				};
			auto credit = ecs.getComponent<Components::Input>(main->credit.button);
			credit->onClick = [this]
				{
					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false};
						this->current_menu->to_credit();
						this->transition = true;
					}
				};
			auto exit = ecs.getComponent<Components::Input>(main->exit.button);
			exit->onClick = [this]
				{
					if (!this->transition)
					{
						gGameStateNext = GameStates::GS_QUIT;
					}
				};
			break;
		}

		case (MenuUI::CURRENT_MENU::PLAY):
		{
			this->current_menu = new PlayMenu;
			this->current_menu->dest = BaseMenu::DESTINATION::NONE;
			this->current_menu->init();

			auto playmenu = dynamic_cast<PlayMenu*>(this->current_menu);

			auto continue_game = ecs.getComponent<Components::Input>(playmenu->continue_game.button);

			if(gameData.new_Start)
			{
				auto continue_game_color = ecs.getComponent<Components::Color>(playmenu->continue_game.button);
				continue_game_color->c_color.a = 0.3f;
				continue_game_color->d_color.a = 0.3f;
				continue_game->allow_hover = false;
			}
			else
			{
				continue_game->onClick = [this]
					{
						if (!this->transition)
						{
							this->fade.free();
							this->fade = UIO::ScreenTransition{ false, 1.f };
							this->current_menu->to_game();
							this->transition = true;
						}
					};
			}

			auto new_game = ecs.getComponent<Components::Input>(playmenu->new_game.button);
			new_game->onClick = [this]
				{
					if (!gameData.new_Start)
					{
						if (!this->transition)
						{
							this->fade.free();
							this->fade = UIO::ScreenTransition{ false};
							this->current_menu->to_confirm();
							this->transition = true;
						}
					}
					else
					{
						if (!this->transition)
						{
							this->fade.free();
							this->fade = UIO::ScreenTransition{ false};
							this->current_menu->to_game();
							this->transition = true;
						}
					}
				};
			auto tutorial = ecs.getComponent<Components::Input>(playmenu->tutorial.button);
			tutorial->onClick = [this]
				{
					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false };
						this->current_menu->to_tutorial();
						this->transition = true;
					}
				};
			auto exit = ecs.getComponent<Components::Input>(playmenu->exit.button);
			exit->onClick = [this]
				{
					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false };
						this->current_menu->to_main();
						this->transition = true;
					}
				};
			break;
		}

		case (MenuUI::CURRENT_MENU::SETTING):
		{
			this->current_menu = new SettingMenu;
			this->current_menu->dest = BaseMenu::DESTINATION::NONE;
			this->current_menu->init();

			auto setting = dynamic_cast<SettingMenu*>(this->current_menu);

			auto exit = ecs.getComponent<Components::Input>(setting->exit.button);
			exit->onClick = [this]()
				{

					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false};
						this->current_menu->to_main();
						this->transition = true;
					}
				};
			break;
		}
		case (MenuUI::CURRENT_MENU::CREDIT):
		{
			this->current_menu = new CreditMenu;
			this->current_menu->dest = BaseMenu::DESTINATION::NONE;
			this->current_menu->init();

			auto credit = dynamic_cast<CreditMenu*>(this->current_menu);


			auto exit = ecs.getComponent<Components::Input>(credit->exit.button);
			exit->onClick = [this]()
				{

					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false };
						this->current_menu->to_main();
						this->transition = true;
					}
				};
			break;
		}
		case (MenuUI::CURRENT_MENU::CONFIRM):
		{
			this->current_menu = new ConfirmMenu;
			this->current_menu->dest = BaseMenu::DESTINATION::NONE;
			this->current_menu->init();

			auto credit = dynamic_cast<ConfirmMenu*>(this->current_menu);


			auto yes = ecs.getComponent<Components::Input>(credit->yes.button);
			yes->onClick = [this]()
				{

					if (!this->transition)
					{
						gameData.new_Start = true;
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false };
						this->current_menu->to_game();
						this->transition = true;
					}
				};
			auto no = ecs.getComponent<Components::Input>(credit->no.button);
			no->onClick = [this]()
				{

					if (!this->transition)
					{
						this->fade.free();
						this->fade = UIO::ScreenTransition{ false };
						this->current_menu->to_main();
						this->transition = true;
					}
				};
			break;
		}
	}


}

void MenuUI::update()
{

	switch (this->current_menu->dest)
	{

		case(BaseMenu::DESTINATION::GAME):
		{
			auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
			if (!this->fade.update())
			{
				this->free();
				this->cur = CURRENT_MENU::MAIN;
				gLevelStateNext = LevelStates::LS_COMBAT;
				gGameStateNext = GameStates::GS_Game;
				//this->current_menu->dest = BaseMenu::DESTINATION::NONE;
				this->transition = false;
			}
			break;
		}

		case(BaseMenu::DESTINATION::MAIN):
		{
			auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
			if (!this->fade.update())
			{
				this->cur = CURRENT_MENU::MAIN;
				this->free();
				this->init();
				this->transition = false;
			}
			break;
		}

		case(BaseMenu::DESTINATION::CONFIRM):
		{
			auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
			if (!this->fade.update())
			{
				this->cur = CURRENT_MENU::CONFIRM;
				this->free();
				this->init();
				this->transition = false;

			}
			break;
		}

		case(BaseMenu::DESTINATION::SETTING):
		{
			auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
			if (!this->fade.update())
			{
				this->cur = CURRENT_MENU::SETTING;
				this->free();
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
				this->cur = CURRENT_MENU::CREDIT;
				this->free();
				this->init();
				this->transition = false;

			}
			break;
		}
		case(BaseMenu::DESTINATION::PLAY):
		{
			auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
			if (!this->fade.update())
			{
				this->cur = CURRENT_MENU::PLAY;
				this->free();
				this->init();
				this->transition = false;

			}
			break;
		}
		case(BaseMenu::DESTINATION::TUTORIAL):
		{
			auto timer = ecs.getComponent<Components::Timer>(this->fade.dim);
			if (!this->fade.update())
			{
				std::cout << "[MenuUI] Going to tutorial level\n";
				this->cur = CURRENT_MENU::MAIN;
				gLevelStateNext = LevelStates::LS_TUTORIAL;
				gGameStateNext = GameStates::GS_Game;
				this->free();
				//this->init();
				this->transition = false;

			}
			break;
		}
		default:
			if (!this->fade.update())
			{
				this->transition = false;
			};
			this->current_menu->update();
			break;

	}

}

void SettingMenu::update()
{
	this->amb_slider.update();
	this->sfx_slider.update();
	this->bgm_slider.update();

	AF.bgm.setVolume(bgm_slider.current);
	AF.sfx.setVolume(sfx_slider.current);
	AF.amb.setVolume(amb_slider.current);
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
	this->bgm_slider = UIO::Slider{ start_x + text_slider_gap,  start_y - offset_y * 0, s_w, s_h, 1200, nullptr, AF.bgm.volume, 1.f};

	this->amb_text = UIO::TextShadow{ start_x,start_y - offset_y * 1 - s_h * text_size,  text_size, 1200 , "Ambience" , 0xFFFFFFFF };
	this->amb_slider = UIO::Slider{ start_x + text_slider_gap,  start_y - offset_y * 1, s_w, s_h, 1200, nullptr, AF.amb.volume, 1.f };

	this->sfx_text = UIO::TextShadow{ start_x, start_y - offset_y * 2 - s_h * text_size, text_size, 1200 , "SFX" , 0xFFFFFFFF };
	this->sfx_slider = UIO::Slider{ start_x + text_slider_gap ,  start_y - offset_y * 2, s_w, s_h, 1200, nullptr, AF.sfx.volume, 1.f };
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
	   
	this->list.reserve(30);
	std::vector<const char*> buffer
	{
		"BEYOND THE NEXUS", //0
		"by Immaculate Js", 

		"Team Members:", //2
		"Pham Minh Tuan",
		"Wai Pyoo Ooo",
		"Zejin Kendreich Dayap Chen",
		"Tio Chun Yi",

		"Faculty and Advisors:", //7
		"Prof. Gerald Wong Han Feng",
		"Prof. Tommy Tan Chee Wei",
		"Dr. Soroor Malekmohammadai Faradounbeh",

		"Create at:", //11
		"DIGIPEN", //12
		"All content (c) 2026 DigiPen Institute of Technology Singapore. All Rights Reserved",

		"PRESIDENTS:", //14
		"CLAUDE COMAIR",
		"EXECUTIVES:",
		"JASON CHU - SAMIR ABOU SAMRA - MICHELE COMAIR",
		"ANGELA KUGLER - ERIK MOHRMANN",
		"BENJAMIN ELLINGER - MELVIN GONSALVEZ",

		"SFX/Music composed by:", //20
		"PHAM MINH TUAN",
		"Art Assets by:",//22
		"Tio Chun Yi",
		//24
		"SPECIAL THANKS TO YOU", "THANK YOU FOR PLAYING THE GAME", "See you in the future!"

		/*"YOUCANTESCAPEYOUCANTESCAPEYOUCANTESCAPE"
		"YOUCANTESCAPEYOawNTESCAPEYOUCANTESCAPE"
		"YOUCANTESCAPEYOUCANTESCawdYOUCANTESCAPE"
		"YOUCAawdCAPEYOUCANTESCAPEYOUCANTESCAPE"
		"YOUCANTESCAPEYOUCANTESCAadwCANTESCAPE"
		"YOUC!@#TESC@#  & HU(CANTESC!@#&@ANTESCAPE"
		"!@#*&)@ & ESCAPE^!@#*(ANTESCAPEYOUC!)@* #JD - AN"
		"OUCPAWOD +!@_#HU(CANTESCA!)@@#)D(0ANTESCAPE"
		"190283EWIUO ESCAPEYOUCANTESCAPEYOUC!)AW819 AN"*/

	};

	this->offset_y = 0;
	this->speed = 100.f;

	f32 start_y = AEGfxGetWinMinY();
	f32 gap_y = 64.f * 2.f;
	f32 size_x = 1.3f;
	f32 size_y = 1.5f;

	int i = 0;
	for(const char* s : buffer)
	{
		f32 text_size = 0.5f;
		if(i == 0 || i == 2 || i == 7|| i == 11|| i == 12 ||  i == 14 || i == 16 || i == 20|| i == 22 || i > 24 ) text_size = 1.f;
		
		f32 text_w, text_h;
		AEGfxGetPrintSize(TF.getFontID(), s, text_size, &text_w, &text_h);

		this->list.push_back( new UIO::TextShadow( -text_w * AEGfxGetWinMaxX() * 0.5f , start_y - gap_y * i, text_size, this->z, s, {1.f, 1.f, 1.f, 1.f} ));
		i++;
	}


	this->exit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMaxX() - 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, 0.5f ,0.f, 1200, "GO BACK", nullptr, 0xFFFFFFFF };

}

void CreditMenu::update()
{
	if(this->offset_y >= AEGfxGetWindowHeight() + 64.f * 2.f * this->list.size()) this->dest = DESTINATION::MAIN;
	this->offset_y += static_cast<f32>(AEFrameRateControllerGetFrameTime()) * this->speed;
	for(UIO::TextShadow* t : this->list)
	{
		auto transform = ecs.getComponent<Components::Transform>(t->text);
		auto transform2 = ecs.getComponent<Components::Transform>(t->text_shadow);

		transform->pos_onscreen.y = transform->pos.y + this->offset_y;
		transform2->pos_onscreen.y = transform2->pos.y + this->offset_y;

	}
}


void CreditMenu::free()
{

	std::cout << "CreditMenu::free() called, list size: " << this->list.size() << '\n';
	for(UIO::TextShadow* t : this->list)
	{
		t->free();
		delete t;
	}
	this->list.clear();
	exit.free();
}

void ConfirmMenu::init()
{
	f32 size_x = 1.3f;
	f32 size_y = 1.5f;


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

	f32 win_h = AEGfxGetWindowHeight();
	AEGfxGetPrintSize(TF.getFontID(), warning_text, text_size_warning, &warning_w, &warning_h);
	AEGfxGetPrintSize(TF.getFontID(), description_text, text_size, &description_w, &description_h);
	//AEGfxGetPrintSize(TF.getFontID(), description_text2, text_size, &description2_w, &description2_h );
	AEGfxGetPrintSize(TF.getFontID(), question_text, text_size, &question_w, &question_h);

	f32 start_y = AEGfxGetWinMaxY() - win_h * 0.1;

	this->warning = UIO::TextShadow{ -warning_w * win_w, start_y - offset_y, text_size_warning, 1200, warning_text, {1.f, 0.f, 0.f, 1.f} };
	this->description = UIO::TextShadow{ -description_w * win_w , start_y - offset_y * 2, text_size, 1200, description_text, {1.f, 1.f, 1.f, 1.f} };
	this->question = UIO::TextShadow{ -question_w * win_w, start_y - offset_y * 4, text_size, 1200, question_text, {1.f, 0.f, 0.f, 1.f} };

	this->yes = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, text_size ,0.f, 1200, "Yes", nullptr, 0xFFFFFFFF };
	this->no = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMaxX() - 250.f, AEGfxGetWinMinY() + 100.f,256.f * size_x, 61.f * size_y, text_size ,0.f, 1200, "No", nullptr, 0xFFFFFFFF };
}
void ConfirmMenu::free()
{
	yes.free();
	no.free();
	warning.free();
	description.free();
	question.free();
}

void MenuUI::free()
{
	if (this->current_menu != nullptr)
	{
		this->current_menu->free();
		delete current_menu;
		this->current_menu = nullptr;
	}

	this->fade.free();

	if (this->background != 0)
	{
		ecs.destroyEntity(this->background);
		this->background = 0;
	}
}


void MainMenu::free()
{
	this->play.free();
	this->setting.free();
	this->exit.free();
	this->credit.free();
	ecs.destroyEntity(this->title);
}

void PlayMenu::free()
{
	this->continue_game.free();
	this->new_game.free();
	this->exit.free();
	this->tutorial.free();
	ecs.destroyEntity(this->title);
}

void PlayMenu::init()
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


	this->continue_game = UIO::TextureButton{ TF.getTextureUI(11) ,AEGfxGetWinMinX() + start_x,  start_y, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "CONTINUE", nullptr, 0xFFFFFFFF };
	this->new_game = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + start_x + offset_x, start_y + offset_y, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "NEW GAME", nullptr, 0xFFFFFFFF };
	this->tutorial = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + start_x + offset_x * 2, start_y + offset_y * 2, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "TUTORIAL", nullptr, 0xFFFFFFFF };
	this->exit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + start_x + offset_x * 3, start_y + offset_y * 3,256.f * size_x, 61.f * size_y, text_size ,0.f, 1200, "BACK", nullptr, 0xFFFFFFFF };


	this->title = UIO::ui_blank_texture(TF.getTextureOthers(9), 0, 100.f, 957 * title_s_x, 513 * title_s_y, 0.f, 1200);

}


