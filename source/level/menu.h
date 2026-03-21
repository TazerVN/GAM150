#pragma once
#include "AEEngine.h"

static bool shouldQuit = false;
static AEGfxTexture* bgTexture = nullptr;

struct Button {
    float x, y, width, height;
    unsigned int color;
    unsigned int hoverColor;
};

static Button playBtn;
static Button quitBtn;
static s8 menuFont;

void GameStateMainMenu_load();
void GameStateMainMenu_init();
void GameStateMainMenu_update();
void GameStateMainMenu_free();
void GameStateMainMenu_unload();

bool IsMouseOver(const Button& btn);
void DrawButton(const Button& btn);

