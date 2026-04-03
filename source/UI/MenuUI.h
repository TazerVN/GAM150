#include "UIObject.h"
#include "util/GameStateManager.h"

class BaseMenu
{
	public:

	enum class DESTINATION
	{
		GAME, CONFIRM, PLAY ,SETTING, CREDIT, MAIN, NONE
	};
	DESTINATION dest;
	void to_main();
	void to_play();
	void to_confirm();
	void to_game();
	void to_setting();
	void to_credit();
	BaseMenu();
	virtual void init() = 0;
	virtual void free() = 0;
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
	UIO::TextureButton exit;
	void init();
	void free();
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
};

class ConfirmMenu : public BaseMenu
{
public:
	UIO::TextureButton yes;
	UIO::TextureButton no;
	void init();
	void free();
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


