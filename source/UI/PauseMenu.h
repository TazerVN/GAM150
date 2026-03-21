#include "types.h"


class PauseMenu
{
	private:
	Entity dim;
	Entity continue_button;
	Entity restart_button;
	Entity leave_button;
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

