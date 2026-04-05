#include "pch.h"
#include "logo.h"
#include "UI/UIObject.h"
#include "../util/GameStateManager.h"

UIO::ScreenTransition fade;
Entity logo{0};
bool stay = false;;

void GameStateLogo_load()
{
}
void GameStateLogo_init()
{
	f32 size = 0.7f;
	logo = UIO::ui_blank_texture_world(TF.getTextureOthers(12), 0, 0, 1525 * size, 445 * size, 0, 1200);
	fade = UIO::ScreenTransition{true, 0.f, 1.f, 1.f};
}
void GameStateLogo_update()
{
	if(fade.fadeIn == true && !fade.update())
	{
		if(stay == false)
		{
			stay = true;
			fade = UIO::ScreenTransition{ true, 0.f, 0.f, 2.f };
		}
		else
		{
			fade = UIO::ScreenTransition{ false, 0.f, 1.f, 1.f };
		}
	}
	else if(fade.fadeIn == false && !fade.update()) 
	{
		gGameStateNext = GameStates::GS_MAINMENU;
	}
}
void GameStateLogo_free()
{

}
void GameStateLogo_unload()
{
	fade.free();
	if(logo != 0) ecs.destroyEntity(logo);
}