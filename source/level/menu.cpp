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
	std::cout << "Main menu loaded" << '\n';

}
void GameStateMainMenu_init()
{
	std::cout << "Main menu init" << '\n';

    // Load font (second param is font size)
    menuFont = AEGfxCreateFont("Assets/font/cool.ttf", 72);
    bgTexture = AEGfxTextureLoad("Assets/others/Gradient.png");

    UIM.menu_init();

    ecs.remove_empty_groups();
    shouldQuit = false;
}
void GameStateMainMenu_update()
{
	if (AEInputCheckTriggered(AEVK_1))
		gGameStateNext = GameStates::GS_Game;
	if (AEInputCheckTriggered(AEVK_Q))
		gGameStateNext = GameStates::GS_QUIT;

  

    
    AEGfxSetBackgroundColor(0.f, 0.f, 0.f);

   
    UIM.menu.update();

}
void GameStateMainMenu_free()
{
	std::cout << "Main menu free" << '\n';
    UIM.menu.free();
}
void GameStateMainMenu_unload()
{
    std::cout << "Main menu unloaded" << '\n';
    AEGfxDestroyFont(menuFont);
    AEGfxTextureUnload(bgTexture);
}

bool IsMouseOver(const Button& btn)
{
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    // Convert screen coords (top-left origin) to world coords (center origin)
    f32 wx = (f32)mx - AEGfxGetWindowWidth() / 2.f;
    f32 wy = AEGfxGetWindowHeight() / 2.f - (f32)my;

    return fabsf(wx - btn.x) <= btn.width / 2.f &&
        fabsf(wy - btn.y) <= btn.height / 2.f;
}

void DrawButton(const Button& btn)
{
    unsigned int col = IsMouseOver(btn) ? btn.hoverColor : btn.color;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    AEGfxSetColorToMultiply(
        ((col >> 16) & 0xFF) / 255.f,   // R
        ((col >> 8) & 0xFF) / 255.f,   // G
        ((col >> 0) & 0xFF) / 255.f,   // B
        ((col >> 24) & 0xFF) / 255.f    // A
    );

    AEMtx33 t;
    AEMtx33Scale(&t, btn.width, btn.height);
    AEMtx33TransApply(&t, &t, btn.x, btn.y);
    AEGfxSetTransform(t.m);

    // Use MeshFactory's rectangle instead of a raw quad
    AEGfxMeshDraw(mf.MeshGet(MESH_RECTANGLE_CENTER), AE_GFX_MDM_TRIANGLES);
}

