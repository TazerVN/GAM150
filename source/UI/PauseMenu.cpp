#include "pch.h"
#include "PauseMenu.h"
#include "../UI/UIObject.h"
#include "AEEngine.h"

PauseMenu::PauseMenu(EntityComponent::Registry& ecs, MeshFactory& mf, s32 z) 
	: on{ false }, created{false}, dim {}, continue_button{}, restart_button{}, leave_button{}
{
	Components::TagClass tag{ Components::Tag::UI };
	this->dim = UIO::ui_blank(ecs, mf, -AEGfxGetWindowWidth() /2.f, AEGfxGetWindowHeight()/2.f, AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), 0, z, 0.0f, 0.0f, 0.0f, 0.2f);
	this->continue_button = UIO::ui_button(ecs, mf, 0, AEGfxGetWinMaxY()/ 3.f, 300.f, 100.f, 0, z + 1, nullptr );
	this->restart_button = UIO::ui_button(ecs, mf, 0, 0, 300.f, 100.f, 0, z + 1, nullptr);
	this->leave_button = UIO::ui_button(ecs, mf, 0, -AEGfxGetWinMaxY() / 3.f, 300.f, 100.f, 0, z + 1, nullptr);
	ecs.addComponent(this->dim, tag);
	ecs.addComponent(this->continue_button, tag);
	ecs.addComponent(this->restart_button, tag);
	ecs.addComponent(this->leave_button, tag);
}

PauseMenu& PauseMenu::operator=(const PauseMenu& rhs)
{
	this->dim = rhs.dim;
	this->continue_button = rhs.continue_button;
	this->restart_button = rhs.restart_button;
	this->leave_button = rhs.leave_button;
	this->created = true;

	auto button = ecs.getComponent<Components::Input>(this->continue_button);
	button->onClick = [this] { this->on = false; };

	return *this;
}

void PauseMenu::update(EntityComponent::Registry& ecs)
{
	//auto button = ecs.getComponent<Components::Input>(this->continue_button);
	//button->onClick = [this]{ this->on; };
}

void PauseMenu::free(EntityComponent::Registry& ecs)
{
	if(this->dim != 0){
		ecs.destroyEntity(this->dim);
		this->dim = 0;
	}
	if(this->continue_button != 0){
		ecs.destroyEntity(this->continue_button);
		this->continue_button = 0;
	}
	if(this->restart_button != 0){
		ecs.destroyEntity(this->restart_button);
		this->restart_button = 0;
	}
	if(this->leave_button != 0){
		ecs.destroyEntity(this->leave_button);
		this->leave_button = 0;
	}
	this->created = false;
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