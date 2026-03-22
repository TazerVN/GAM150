#include "pch.h"
#include "PauseMenu.h"
#include "../UI/UIObject.h"
#include "AEEngine.h"

PauseMenu::PauseMenu(s32 z) 
	: on{ false }, created{false}, dim {}, continue_button{}, restart_button{}, leave_button{}
{
	Components::TagClass tag{ Components::Tag::UI };
	Components::Input in{AEVK_LBUTTON, true, nullptr, nullptr, nullptr, 40};

	this->dim = UIO::ui_blank_solid_center(0, 0, AEGfxGetWindowWidth() * 1.5f, AEGfxGetWindowHeight() * 1.5f, 0, 40, 0.0f, 0.0f, 0.0f, 0.5f);


	this->continue_button.button = UIO::ui_button(0, AEGfxGetWinMaxY() * 0.50f, 300.f, 100.f, 0, z, nullptr);
	this->continue_button.text = UIO::ui_text(-70.f, AEGfxGetWinMaxY() * 0.50f - 10.f, 0.5f, 100.f, 0, z + 1, "Continue" );
	


	this->restart_button.button = UIO::ui_button(0, 0, 300.f, 100.f, 0, z,nullptr);
	this->restart_button.text = UIO::ui_text(-100.f, -10.f, 0.5f, 100.f, 0, z + 1, "Abandon Run");



	this->leave_button.button = UIO::ui_button(0, AEGfxGetWinMaxY() * -0.50f, 300.f, 100.f, 0, z, nullptr);
	this->leave_button.text = UIO::ui_text(-40.f, AEGfxGetWinMaxY() * -0.50f - 10.f, 0.5f, 100.f, 0, z + 1, "Exit");


	ecs.addComponent(this->dim, tag);
	ecs.addComponent(this->dim, in);
	ecs.addComponent(this->continue_button.button, tag);
	ecs.addComponent(this->continue_button.text, tag);
	ecs.addComponent(this->restart_button.button, tag);
	ecs.addComponent(this->restart_button.text, tag);
	ecs.addComponent(this->leave_button.button, tag);
	ecs.addComponent(this->leave_button.text, tag);
}

PauseMenu& PauseMenu::operator=(const PauseMenu& rhs)
{
	this->dim = rhs.dim;
	this->continue_button = rhs.continue_button;
	this->restart_button = rhs.restart_button;
	this->leave_button = rhs.leave_button;
	this->created = true;

	auto button_c = ecs.getComponent<Components::Input>(this->continue_button.button);
	button_c->onClick = [this] { this->on = false; };

	auto button_r = ecs.getComponent<Components::Input>(this->restart_button.button);
	button_r->onClick = [this] 
	{
		this->on = false;
		gLevelStateNext = LevelStates::LS_RESTART;
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
	this->restart_button.free();
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
void PauseMenu::TextButton::free()
{
	if(this->button != 0)
	{
		ecs.destroyEntity(this->button);
		this->button = 0;
	}
	if (this->text != 0)
	{
		ecs.destroyEntity(this->text);
		this->text = 0;
	}

}


