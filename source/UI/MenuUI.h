#include "UIObject.h"
#include "util/GameStateManager.h"

class BaseMenu
{
	public:
	bool isLeaving;
	UIO::ScreenTransition fade;
	BaseMenu();
};

class MainMenu : public BaseMenu
{
	public:
	Entity title;
	UIO::TextureButton play;
	UIO::TextureButton option;
	UIO::TextureButton credit;
	UIO::TextureButton exit;
	MainMenu();
	void update();
	void init();
	void free();
	void leave();
};

class CreditMenu : public BaseMenu
{

};

class SettingMenu : public BaseMenu
{

};

class MenuUI
{
	public:
	MainMenu main;
	SettingMenu setting;
	CreditMenu credit;
	MenuUI();
	void init();
	void update();
	void free();
	void leave();
};


