#include "inputSystem.h"
namespace InputSystem
{
	void InputManager::update(ECS::Registry& ecs, PhaseSystem::GameBoardState& gbs)
	{

		if(gbs.getPlayerPhase() == PhaseSystem::PlayerPhase::WAITING) return;

		AEInputGetCursorPosition(&this->mousex, &this->mousey);

		this->mousex = this->mousex - f32(AEGfxGetWindowWidth()) * 0.5f;
		this->mousey = -this->mousey + f32(AEGfxGetWindowHeight()) * 0.5f;

		for (int i = 0; i < ecs.sizeEntity(); i++)
		{

			ECS::ComponentTypeID iID = ECS::getComponentTypeID<Components::Input>();
			ECS::ComponentTypeID tID = ECS::getComponentTypeID<Components::Transform>();

			if (!ecs.getBitMask()[i].test(tID)) continue;
			if (!ecs.getBitMask()[i].test(iID)) continue;

			Components::Input* in = ecs.getComponent<Components::Input>(i);

			if(in->on == false) continue;


			Components::Transform* t = ecs.getComponent<Components::Transform>(i);
			Components::Color* c = ecs.getComponent<Components::Color>(i);


			if (point2rect_intersect(t->pos_onscreen.x, t->pos_onscreen.y, t->size_col.x, t->size_col.y, f32(this->mousex), f32(this->mousey)))
			{
				if (AEInputCheckTriggered(in->type))
				{
					//AEVec2Set(&t->pos_onscreen, f32(this->mousex), f32(this->mousey));
					this->cur_in = i;
					if (in->onClick != nullptr) in->onClick();
					//std::cout << "yay";
				}
				else if (AEInputCheckReleased(in->type))
				{
					//in->onclick();
				}
				else if (in->hover == true)
				{

					if (in->onHover != nullptr) in->onHover();
				}
			}
			else
			{
				if (in->offHover != nullptr) in->offHover();

			}


		}

	}

	InputManager::InputManager()
	{
		this->mousex = 0;
		this->mousey = 0;
	}

}