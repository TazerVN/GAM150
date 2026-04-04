#include "types.h"
#include "UI/UIObject.h"


class BasePauseMenu
{
	public:
	enum class DESTINATION
	{
		MAIN, CONTINUE, CONFIRM, TUTORIAL, SETTING, EXIT
	};
	DESTINATION dest;
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
};

class EmptyPause : public BasePauseMenu
{

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

//class TutorialPause : public BasePauseMenu
//{
//	public:
//	UIO::TextureButton leave_button;
//	void free();
//};
//
//class ConfirmPause : public BasePauseMenu
//{
//	public:
//	UIO::TextureButton leave_button;
//	void free();
//};

//class SettingPause : public BasePauseMenu
//{
//	public:
//	UIO::TextShadow bgm_text;
//	UIO::TextShadow amb_text;
//	UIO::TextShadow sfx_text;
//
//	UIO::Slider bgm_slider;
//	UIO::Slider amb_slider;
//	UIO::Slider sfx_slider;
//
//	UIO::TextureButton leave;
//	void init();
//	void free();
//	void update() override;
//	void free();
//};

class PauseMenu
{
	private:
	
	s32 z{1300};
	Entity dim{0};
	Entity timer{ 0 };
	BasePauseMenu* menu;
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

