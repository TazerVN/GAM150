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
    this->credit.free();
    ecs.destroyEntity(this->title);
    this->fade.free();
}

void MenuUI::init()
{

    f32 start_y = -300.f;
    f32 start_x = 300.f;

    f32 size_x = 1.3f;
    f32 size_y = 1.3f;


    f32 offset_x = 256.f * size_x;
    f32 offset_y = 0.f;
    f32 title_s_x = 1.f;
    f32 title_s_y = 1.f;

    f32 text_size = 0.5f;

    /*f32 start_y = -50.f;
    f32 offset_x = AEGfxGetWinMaxX();
    f32 offset_y = -100.f;
    f32 size_x = 1.5f;
    f32 size_y = 1.5f;

    f32 title_s_x = 1.f;
    f32 title_s_y = 1.f;*/

    this->fade = UIO::ScreenTransition(true);
    this->play = UIO::TextureButton{ TF.getTextureUI(11) ,AEGfxGetWinMinX() + start_x,  start_y, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "PLAY", nullptr, 0xFF12CCFF };
    this->option = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() +  start_x + offset_x, start_y + offset_y, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "SETTING", nullptr, 0xFF12CCFF };
    this->credit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() +  start_x + offset_x * 2, start_y + offset_y * 2, 256.f * size_x, 61.f * size_y, text_size, 0.f, 1200, "CREDIT", nullptr, 0xFF12CCFF };
    this->exit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + start_x + offset_x * 3, start_y + offset_y * 3,256.f * size_x, 61.f * size_y, text_size ,0.f, 1200, "EXIT", nullptr, 0xFFCC66FF };

    //this->play = UIO::TextureButton{ TF.getTextureUI(11) ,AEGfxGetWinMinX() + offset_x,  start_y, 256.f * size_x, 61.f * size_y, 0.6f, 0.f, 1200, "PLAY", nullptr, 0xFF12CCFF };
    //this->option = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() +  offset_x, start_y + offset_y, 256.f * size_x, 61.f * size_y, 0.6f, 0.f, 1200, "SETTING", nullptr, 0xFF12CCFF };
    //this->credit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() +  offset_x, start_y + offset_y * 2, 256.f * size_x, 61.f * size_y, 0.6f, 0.f, 1200, "CREDIT", nullptr, 0xFF12CCFF };
    //this->exit = UIO::TextureButton{ TF.getTextureUI(11) , AEGfxGetWinMinX() + offset_x, start_y + offset_y * 3,256.f * size_x, 61.f * size_y, 0.6f ,0.f, 1200, "EXIT", nullptr, 0xFFCC66FF };

    this->title = UIO::ui_blank_texture( TF.getTextureOthers(9),0 , 100.f, 957 * title_s_x, 513 * title_s_y, 0.f, 1200);
}

void MenuUI::update()
{
    this->fade.update();
}