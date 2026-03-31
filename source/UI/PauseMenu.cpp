#include "pch.h"
#include "PauseMenu.h"
#include "../UI/UIObject.h"
#include "AEEngine.h"

PauseMenu::PauseMenu(s32 z) 
	: on{ false }, created{false}, dim {}, continue_button{}, abandon_button{}, leave_button{}, setting_button{}
{

	f32 size_x = 1.4f;
	f32 size_y = 1.8f;

	f32 start_y = 250.f;
	f32 start_x =/* AEGfxGetWinMinX() + 200.f * size_x*/ 0.f ;
	f32 offset_y = 100.f * size_y;

	this->dim = UIO::ui_blank_solid_center(0, 0, AEGfxGetWindowWidth() * 1.5f, AEGfxGetWindowHeight() * 1.5f, 0, z, 0.0f, 0.0f, 0.0f, 0.5f);

	Components::TagClass tag{ Components::Tag::UI };
	Components::Input in{AEVK_LBUTTON, true, nullptr, nullptr, nullptr, z, true};

	/*this->continue_button.button = UIO::ui_button(0, AEGfxGetWinMaxY() * 0.50f, 300.f, 100.f, 0, z, nullptr);
	this->continue_button.text = UIO::ui_text(-70.f, AEGfxGetWinMaxY() * 0.50f - 10.f, 0.5f, 100.f, 0, z + 1, "Continue" );*/
	this->continue_button = UIO::TextureButton(TF.getTextureUI(12),start_x, start_y - offset_y * 0, 314.f * size_x, 65.f * size_y, 0.5f ,0, z + 1,  "Continue" ,nullptr, {1.f,1.f,1.f,1.f});


	/*this->abandon_button.button = UIO::ui_button(0, 0, 300.f, 100.f, 0, z,nullptr);
	this->abandon_button.text = UIO::ui_text(-100.f, -10.f, 0.5f, 100.f, 0, z + 1, "Abandon Run");*/
	this->abandon_button = UIO::TextureButton(TF.getTextureUI(12), start_x, start_y - offset_y * 1, 314.f * size_x, 65.f * size_y, 0.5f, 0, z + 1, "Abandon", nullptr, { 1.f,1.f,1.f,1.f });
	/*this->abandon_button.button = UIO::ui_button(0, 0, 300.f, 100.f, 0, z,nullptr);
	this->abandon_button.text = UIO::ui_text(-100.f, -10.f, 0.5f, 100.f, 0, z + 1, "Abandon Run");*/
	this->setting_button = UIO::TextureButton(TF.getTextureUI(12), start_x, start_y - offset_y * 2, 314.f * size_x, 65.f * size_y, 0.5f, 0, z + 1, "Setting", nullptr, { 1.f,1.f,1.f,1.f });


	//this->leave_button.button = UIO::ui_button(0, AEGfxGetWinMaxY() * -0.50f, 300.f, 100.f, 0, z, nullptr);
	//this->leave_button.text = UIO::ui_text(-40.f, AEGfxGetWinMaxY() * -0.50f - 10.f, 0.5f, 100.f, 0, z + 1, "Exit");
	this->leave_button = UIO::TextureButton(TF.getTextureUI(12), start_x, start_y - offset_y * 3, 314.f * size_x, 65.f * size_y, 0.5f, 0, z + 1, "Exit", nullptr, { 1.f,1.f,1.f,1.f });

	ecs.addComponent(this->dim, tag);
	ecs.addComponent(this->dim, in);
}

PauseMenu& PauseMenu::operator=(const PauseMenu& rhs)
{
	this->dim = rhs.dim;
	this->continue_button = rhs.continue_button;
	this->abandon_button = rhs.abandon_button;
	this->leave_button = rhs.leave_button;
	this->setting_button = rhs.setting_button;
	this->created = true;

	auto button_c = ecs.getComponent<Components::Input>(this->continue_button.button);
	button_c->onClick = [this] { this->on = false; };

	auto button_r = ecs.getComponent<Components::Input>(this->abandon_button.button);
	button_r->onClick = [this] 
	{
		this->on = false;
		player_died = true;
	};

	auto button_l = ecs.getComponent<Components::Input>(this->leave_button.button);
	button_l->onClick = [this]
	{
		this->on = false;
		gLevelStateNext = LevelStates::LS_QUIT;
	};

	return *this;
}

void PauseMenu::update()
{
	//auto button = ecs.getComponent<Components::Input>(this->continue_button);
	//button->onClick = [this]{ this->on; };
}

void PauseMenu::free()
{
	if(this->dim != 0){
		ecs.destroyEntity(this->dim);
		this->dim = 0;
	}
	this->continue_button.free();
	this->abandon_button.free();
	this->setting_button.free();
	this->leave_button.free();
	this->created = false;
	this->on = false;
}
bool PauseMenu::isOn()
{
	return on;
}

bool PauseMenu::isCreated()
{
	return created;
}

void PauseMenu::setStateOn(bool flag)
{
	on = flag;
}
void PauseMenu::setStateCreate(bool flag)
{
	created = flag;
}



