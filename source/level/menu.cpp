#include "pch.h"
#include "menu.h"
#include "../global.h"
#include <iostream>
#include "AEEngine.h"
#include "../util/GameStateManager.h"
#include "../UI/UIObject.h"
#include "../UI/UI.h"
//#include "../system/particleSystem.h"

static UI::UIManager UIM;
// PROXY CODE
// NEED CLEAN UP
//Particle::ParticleSystem PS;

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




