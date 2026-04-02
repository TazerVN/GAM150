#include "UIObject.h"
#include "util/GameStateManager.h"

class BaseMenu
{
	public:

	enum class DESTINATION
	{
		GAME, CONTINUE ,SETTING, CREDIT, MAIN, NONE
	};
	DESTINATION dest;
	void to_main();
	void to_continue();
	void to_game();
	void to_setting();
	void to_credit();
	BaseMenu();
};

class MainMenu : public BaseMenu
{
	public:
	Entity title{};
	UIO::TextureButton play;
	UIO::TextureButton setting;
	UIO::TextureButton credit;
	UIO::TextureButton exit;
	MainMenu();
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

class ContinueLastRun : public BaseMenu
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
		MAIN, CONTINUE_LAST_RUN , SETTING, CREDIT
	};

	UIO::ScreenTransition fade;

	CURRENT_MENU cur;
	bool transition;

	MainMenu main;
	ContinueLastRun continueLastRun;
	SettingMenu setting;
	CreditMenu credit;
	MenuUI();
	void init();
	void update();
	void free();
};


