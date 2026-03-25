#include "types.h"


class PauseMenu
{
	private:
	struct TextButton
	{
		Entity text{};
		Entity button{};
		void free();
	};

	s32 z{1200};
	Entity dim;
	TextButton continue_button;
	TextButton abandon_button;
	TextButton leave_button;
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

