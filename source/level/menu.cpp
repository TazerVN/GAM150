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
    //PS.particleDataFlow(ecs, mf);
    //PS.particleDataBubble(ecs, mf);

    //Sam here is the example to use it below call that function whenever u are done VV
    //AEGfxTextureUnload(bgTexture);
    //playBtn = { 0.f,  -50.f, 200.f, 60.f, 0xFF44AA44, 0xFF66CC66 };  // green
    //quitBtn = { 0.f, -150.f, 200.f, 60.f, 0xFFAA4444, 0xFFCC6666 };  // red

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

    if (AEInputCheckTriggered(AEVK_LBUTTON))
    {
        //if (IsMouseOver(playBtn)) //leaveGameState();
        //    gGameStateNext = GameStates::GS_Game;
        //if (IsMouseOver(quitBtn)) shouldQuit = true;
    }

    
    AEGfxSetBackgroundColor(0.f, 0.f, 0.f);

    // DataFlow HERE
    //PS.particleDataFlow(ecs, mf);

    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
    AEGfxTextureSet(bgTexture, 0.f, 0.f);

    AEMtx33 bg;
    AEMtx33Scale(&bg, f32(AEGfxGetWindowWidth()), f32(AEGfxGetWindowHeight()));
    AEMtx33TransApply(&bg, &bg, 0.f, 0.f);   // © 0,0 = center of screen
    AEGfxSetTransform(bg.m);
    AEGfxMeshDraw(mf.MeshGet(MESH_RECTANGLE_CENTER), AE_GFX_MDM_TRIANGLES);
    /*
    if (shouldQuit || 0 == AESysDoesWindowExist())
    {
        AESysExit();
        return;
    }
    */
    UIM.menu.update();


    // DataBub HERE
    //PS.particleDataBubble(ecs, mf);

    //DrawButton(playBtn);
    //DrawButton(quitBtn);

    //AEGfxPrint(menuFont, "Beyond the Nexus", -0.7f, 0.3f, 1.0f, 1.f, 1.f, 1.f, 1.f);

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

