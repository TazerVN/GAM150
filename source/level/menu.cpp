#include "menu.h"
#include "AEEngine.h"
#include "GameState.h"
#include "../factory/MeshFactory.h"

// PROXY CODE
// NEED CLEAN UP

static MeshFactory mf{};
static bool shouldQuit = false;

struct Button {
    float x, y, width, height;
    unsigned int color;
    unsigned int hoverColor;
};

static Button playBtn;
static Button quitBtn;
static s8 menuFont;

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

void menu_init()
{
    mf.MeshFactoryInit();   // builds all meshes including MESH_RECTANGLE_CENTER

    // Load font (second param is font size)
    menuFont = AEGfxCreateFont("Assets/Fonts/cool.ttf", 72);

    playBtn = { 0.f,  -50.f, 200.f, 60.f, 0xFF44AA44, 0xFF66CC66 };  // green
    quitBtn = { 0.f, -150.f, 200.f, 60.f, 0xFFAA4444, 0xFFCC6666 };  // red

    shouldQuit = false;
}

void menu_update()
{
    AESysFrameStart();

    if (AEInputCheckTriggered(AEVK_LBUTTON))
    {
        if (IsMouseOver(playBtn)) leaveGameState();
        if (IsMouseOver(quitBtn)) shouldQuit = true;
    }

    /*
    if (shouldQuit || 0 == AESysDoesWindowExist())
    {
        AESysExit();
        return;
    }
    */

    AEGfxSetBackgroundColor(0.1f, 0.1f, 0.2f);

    DrawButton(playBtn);
    DrawButton(quitBtn);

    //AEGfxPrint(menuFont, "UNTITLED GAME", -0.2f, 0.3f, 1.5f, 1.f, 1.f, 1.f, 1.f);

    AESysFrameEnd();
}
void menu_exit()
{
    mf.MeshFree();
    AEGfxDestroyFont(menuFont);
}