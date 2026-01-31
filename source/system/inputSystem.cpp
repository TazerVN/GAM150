#include "inputSystem.h"
namespace InputSystem
{
	void InputManager::update(ECS::Registry& ecs)
	{
		AEInputGetCursorPosition(&this->mousex, &this->mousey);
		this->mousex -= f32(AEGfxGetWindowWidth()) * 0.5f;
		this->mousey = -this->mousey + f32(AEGfxGetWindowHeight()) * 0.5f;

		ECS::ComponentStorage<Components::Input>* i_com = ecs.getComponentStorage<Components::Input>();

		for (int i = 0; i < i_com->getCount(); i++)
		{

			ECS::ComponentTypeID iID = ECS::getComponentTypeID<Components::Input>();
			ECS::ComponentTypeID tID = ECS::getComponentTypeID<Components::Transform>();
			if (!ecs.getBitMask()[i].test(iID)) continue;
			if (!ecs.getBitMask()[i].test(tID)) continue;

			Components::Input* in = ecs.getComponent<Components::Input>(i);
			Components::Transform* t = ecs.getComponent<Components::Transform>(i);


			if (point2rect_intersect(t->pos_onscreen.x, t->pos_onscreen.y, t->size.x, t->size.y, f32(this->mousex), f32(this->mousey)))
			{
				if (AEInputCheckCurr(in->type))
				{
					AEVec2Set(&t->pos_onscreen, f32(this->mousex),f32(this->mousey));
					std::cout << "yay" ;
				}
				else if (in->hover == true)
				{
					t->pos_onscreen.y = t->pos.y + t->size.y * 0.1;
				}
			}
			else
			{
				t->pos_onscreen = t->pos;
			}


		}

	}

	InputManager::InputManager()
	{
		this->mousex = 0;
		this->mousey = 0;
	}

}