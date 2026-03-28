#include "UIObject.h"
#include "util/GameStateManager.h"

class MenuUI
{
	public:
	UIO::TextShadow title;
	UIO::TextButton play;
	UIO::TextButton option;
	UIO::TextButton exit;
	UIO::ScreenTransition fade;
	MenuUI();
	void init();
	void update();
	void free();
};
