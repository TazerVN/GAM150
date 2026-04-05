//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
//				Tio Chun Yi
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
//				tio.c@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================

#include "menu.h"
#include "../global.h"
#include <iostream>
#include "AEEngine.h"
#include "../util/GameStateManager.h"
#include "../UI/UIObject.h"
#include "../UI/UI.h"

static UI::UIManager UIM;

void GameStateMainMenu_load()
{

}
void GameStateMainMenu_init()
{
    AF.bgm.stopping = false;
    AF.bgm.play(1);
    UIM.menu_init();

    ecs.remove_empty_groups();
    shouldQuit = false;
}
void GameStateMainMenu_update()
{
   
    UIM.menu.update();

}
void GameStateMainMenu_free()
{
    UIM.menu.free();
    AF.bgm.stop();
}
void GameStateMainMenu_unload()
{
}




