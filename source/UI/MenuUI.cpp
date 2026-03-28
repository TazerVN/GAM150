#include "pch.h"
#include "MenuUI.h"


MenuUI::MenuUI()
{
    //this->play = UIO::TextButton{ 0.f,  -50.f, 300.f, 100.f, 0.6f, 0.f, 1200, "PLAY", [] { gGameStateNext = GameStates::GS_Game; }, 0xFF12CCFF };
    //this->option = UIO::TextButton{ 0.f,  -200.f, 300.f, 100.f, 0.6f, 0.f, 1200, "PLAY", nullptr, 0xFF12CCFF };
    //this->exit=  UIO::TextButton{ 0.f, -350.f, 300.f, 100.f, 0.6f ,0.f, 1200, "EXIT", nullptr, 0xFFCC66FF };

    //this->title = UIO::TextShadow(-0.72f * AEGfxGetWinMaxX(), 0.2f * AEGfxGetWinMaxY(), -0.72f * AEGfxGetWinMaxX(), 0.25f * AEGfxGetWinMaxY(), 2.0f, 1200, "Beyond The Nexus", 0x000000FF);


    ////title
    //UIO::ui_text(-0.72f * AEGfxGetWinMaxX(), 0.2f * AEGfxGetWinMaxY(), 2.0f, 2.0f, 0, 1200, "Beyond The Nexus", 0x000000FF);
    //UIO::ui_text(-0.72f * AEGfxGetWinMaxX(), 0.25f * AEGfxGetWinMaxY(), 2.0f, 2.0f, 0, 1200, "Beyond The Nexus", 0xFFFFFFFF);
}

void MenuUI::free()
{
    this->play.free();
    this->option.free();
    this->exit.free();
    this->title.free();
    this->fade.free();
}

void MenuUI::init()
{
    this->fade = UIO::ScreenTransition(true);
    this->play = UIO::TextButton{ 0.f,  -50.f, 300.f, 100.f, 0.6f, 0.f, 1200, "PLAY", nullptr, 0xFF12CCFF };
    this->option = UIO::TextButton{ 0.f,  -200.f, 300.f, 100.f, 0.6f, 0.f, 1200, "SETTING", nullptr, 0xFF12CCFF };
    this->exit = UIO::TextButton{ 0.f, -350.f, 300.f, 100.f, 0.6f ,0.f, 1200, "EXIT", nullptr, 0xFFCC66FF };

    this->title = UIO::TextShadow(-0.72f * AEGfxGetWinMaxX(), 0.2f * AEGfxGetWinMaxY(), -0.72f * AEGfxGetWinMaxX(), 0.25f * AEGfxGetWinMaxY(), 2.0f, 1200, "Beyond The Nexus", 0x000000FF);
}

void MenuUI::update()
{
    this->fade.update();
}