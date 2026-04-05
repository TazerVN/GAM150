//=========================================
//	AUTHOR:		PHAM MINH TUAN
//	EMAIL:		minhtuan.pham@digipen.edu
//	DATE:		5-4-2026
//=========================================

#include "UIObject.h"
#include "util/GameStateManager.h"
#include <vector>

class BaseMenu
{
	public:

	enum class DESTINATION
	{
		GAME, CONFIRM, PLAY, TUTORIAL ,SETTING, CREDIT, MAIN, NONE
	};
	DESTINATION dest;
	s32 z;
	void to_main();
	void to_play();
	void to_confirm();
	void to_game();
	void to_setting();
	void to_credit();
	void to_tutorial();
	BaseMenu();
	virtual void init() = 0;
	virtual void free() = 0;
	virtual void update(){};
	virtual ~BaseMenu() = default;
};

class MainMenu : public BaseMenu
{
	public:
	Entity title{};
	UIO::TextureButton play;
	UIO::TextureButton setting;
	UIO::TextureButton credit;
	UIO::TextureButton exit;
	void init();
	void free();
};

class PlayMenu : public BaseMenu
{
	public:
	Entity title{};
	UIO::TextureButton new_game;
	UIO::TextureButton continue_game;
	UIO::TextureButton tutorial;
	UIO::TextureButton exit;

	void init();
	void free();
};

class CreditMenu : public BaseMenu
{
	public:
	int speed;
	f32 offset_y;
	std::vector<UIO::TextShadow*> list;
	UIO::TextureButton exit;
	void init();
	void free();
	void update() override;
};

class SettingMenu : public BaseMenu
{
	public:
	UIO::TextShadow bgm_text;
	UIO::TextShadow amb_text;
	UIO::TextShadow sfx_text;

	UIO::Slider bgm_slider;
	UIO::Slider amb_slider;
	UIO::Slider sfx_slider;

	UIO::TextureButton exit;
	void init();
	void free();
	void update() override;
};

class ConfirmMenu : public BaseMenu
{
	public:
	UIO::TextShadow warning;
	UIO::TextShadow description;
	//UIO::TextShadow description2;
	UIO::TextShadow question;


	UIO::TextureButton yes;
	UIO::TextureButton no;
	void init();
	void free();
};

class GameOverUI
{
	enum class NEXT{
		MENU, GAME, NONE
	};

	bool transition{false};
	NEXT dest;
	UIO::ScreenTransition fade;
	Entity background{0};
	UIO::TextButton new_game;
	UIO::TextButton exit;

	public:
	void init();
	void free();
	void update();

};

class MenuUI
{
	public:

	enum class CURRENT_MENU : char 
	{
		MAIN, PLAY, CONFIRM, SETTING, CREDIT
	};

	UIO::ScreenTransition fade;
	Entity background{0};

	CURRENT_MENU cur;
	bool transition;

	BaseMenu* current_menu;

	MenuUI() = default;
	void init();
	void update();
	void free();
};


