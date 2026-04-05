//=========================================
//	AUTHOR:		PHAM MINH TUAN
// 
//	EMAIL:		minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================


#include "types.h"
#include "UI/UIObject.h"


class BasePauseMenu
{
	public:
	enum class DESTINATION
	{
		MAIN, CONTINUE, CONFIRM, TUTORIAL, SETTING, EXIT, GAMEOVER
	};
	DESTINATION dest{DESTINATION::MAIN};
	s32 z{ 1300 };
	virtual void init() {};
	virtual void update() {};
	virtual void free() {};
	virtual ~BasePauseMenu() = default;
	void to_continue();
	void to_main();
	void to_confirm();
	void to_tutorial();
	void to_setting();
	void to_exit();
	void to_gameover();
};


class MainPause : public BasePauseMenu
{
	public:
	UIO::TextureButton continue_button;
	UIO::TextureButton abandon_button;
	UIO::TextureButton setting_button;
	UIO::TextureButton tutorial_button;
	UIO::TextureButton leave_button;
	void init() override;
	void free();
};

class TutorialPause : public BasePauseMenu
{
	public:
	int page{0};
	Entity image{0};
	UIO::TextureButton foward_button;
	UIO::TextureButton leave_button;
	UIO::TextureButton backward_button;
	void init() override;
	void update() override;
	void free();
};

class ConfirmPause : public BasePauseMenu
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

class SettingPause : public BasePauseMenu
{
	public:
	UIO::TextShadow bgm_text;
	UIO::TextShadow amb_text;
	UIO::TextShadow sfx_text;

	UIO::Slider bgm_slider;
	UIO::Slider amb_slider;
	UIO::Slider sfx_slider;

	UIO::TextureButton leave;
	void init();
	void update() override;
	void free();
};

class PauseMenu
{
	private:
	
	s32 z{1300};
	UIO::ScreenTransition dim;
	Entity timer{ 0 };
	BasePauseMenu* menu{nullptr};
	bool on{false};
	bool created{false};
	bool transition{false};
	public:

	enum class CURRENT
	{
		MAIN, SETTING, CONFIRM, TUTORIAL, EMPTY
	};

	CURRENT current_menu{CURRENT::EMPTY};
	PauseMenu() = default;
	void init();
	//PauseMenu& operator=(const PauseMenu& rhs);
	void update();
	void free();
	bool isOn();
	bool isCreated();
	void setStateOn(bool flag);
	void setStateCreate(bool flag);

};

