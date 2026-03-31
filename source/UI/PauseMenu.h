#include "types.h"
#include "UI/UIObject.h"


class PauseMenu
{
	private:

	s32 z{1300};
	Entity dim;
	UIO::TextureButton continue_button;
	UIO::TextureButton abandon_button;
	UIO::TextureButton setting_button;
	UIO::TextureButton leave_button;
	bool on{false};
	bool created{false};

	public:
	PauseMenu() = default;
	PauseMenu(s32 z);
	PauseMenu& operator=(const PauseMenu& rhs);
	void update();
	void free();
	bool isOn();
	bool isCreated();
	void setStateOn(bool flag);
	void setStateCreate(bool flag);

};

