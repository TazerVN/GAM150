#include "inputSystem.h"
#include "../system/PhaseSystem.h"
#include <algorithm>

namespace InputSystem
{
	bool IM_CMP(std::pair<s8, Entity> const& rhs, std::pair<s8, Entity> const& lhs);

	void InputManager::update(ECS::Registry& ecs, PhaseSystem::GameBoardState& gbs)
	{
		AEInputGetCursorPosition(&this->mousex, &this->mousey);

		this->mousex = this->mousex - f32(AEGfxGetWindowWidth()) * 0.5f;
		this->mousey = -this->mousey + f32(AEGfxGetWindowHeight()) * 0.5f;

		this->mousex = AEClamp(this->mousex, AEGfxGetWinMinX(), AEGfxGetWinMaxX());
		this->mousey = AEClamp(this->mousey, AEGfxGetWinMinX(), AEGfxGetWinMaxY());

		ECS::ComponentTypeID iID = ECS::getComponentTypeID<Components::Input>();
		//create bitsets
		ECS::ComponentBitMask objMask;
		objMask.set(iID);


		for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				for (Entity ent : it->second)
				{
					Components::Input* input = ecs.getComponent<Components::Input>(ent);
					std::pair<s8, Entity> temp {input->z, ent};
					this->buffer.push_back(temp);
				}
			}
			
		}

		this->buffer.sort(IM_CMP);
		bool active_hover = false;

		for (std::pair<s8, Entity> temp : this->buffer)
		{
			Entity e = temp.second;

			ECS::ComponentTypeID iID = ECS::getComponentTypeID<Components::Input>();
			ECS::ComponentTypeID tID = ECS::getComponentTypeID<Components::Transform>();
			ECS::ComponentTypeID mID = ECS::getComponentTypeID<Components::Mesh>();

			if (!ecs.getBitMask()[e].test(tID)) continue;
			if (!ecs.getBitMask()[e].test(iID)) continue;
			if (!ecs.getBitMask()[e].test(mID)) continue;

			Components::Input* in = ecs.getComponent<Components::Input>(e);

			if(in->on == false) continue;


			Components::Transform* t = ecs.getComponent<Components::Transform>(e);


			if (point2rect_intersect(t->pos_onscreen.x, t->pos_onscreen.y, t->size_col.x, t->size_col.y, f32(this->mousex), f32(this->mousey)))
			{
				if (AEInputCheckTriggered(in->type))
				{
					if (in->onClick != nullptr) in->onClick();
				}
				else if (AEInputCheckReleased(in->type))
				{
					//pass for now
				}
				else if (active_hover == false && in->hover == true)
				{ 
					if (in->onHover != nullptr) {
						in->onHover();
					} 
					active_hover = true;
				}
			}
			else
			{
				if (in->offHover != nullptr) in->offHover();

			}
		}
			this->buffer.clear();

	}

	bool IM_CMP(std::pair<s8, Entity> const& rhs, std::pair<s8, Entity> const& lhs)
	{
		return (rhs.first > lhs.first) ? true : false;
	}

	InputManager::InputManager()
	{
		this->mousex = 0;
		this->mousey = 0;
	}

}