#include "UIObject.h"
#include "util/GameStateManager.h"

class MenuUI
{
	public:
	Entity title;
	UIO::TextureButton play;
	UIO::TextureButton option;
	UIO::TextureButton credit;
	UIO::TextureButton exit;
	UIO::ScreenTransition fade;
	MenuUI();
	void init();
	void update();
	void free();
};
