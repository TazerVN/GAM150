#include "pch.h"
#include "inputSystem.h"
#include "../system/PhaseSystem.h"
#include <algorithm>

namespace InputSystem
{
	bool IM_CMP(std::pair<s32, Entity> const& rhs, std::pair<s32, Entity> const& lhs);

	void InputManager::update(EntityComponent::Registry& ecs, Entity camera_id)
	{
		Components::Transform* cam = ecs.getComponent<Components::Transform>(camera_id);
		AEInputGetCursorPosition(&this->mousex, &this->mousey);

		this->mousex = this->mousex - f32(AEGfxGetWindowWidth()) * 0.5f;
		this->mousey = -this->mousey + f32(AEGfxGetWindowHeight()) * 0.5f;

		this->mousex = AEClamp(this->mousex, AEGfxGetWinMinX(), AEGfxGetWinMaxX());
		this->mousey = AEClamp(this->mousey, AEGfxGetWinMinX(), AEGfxGetWinMaxY());

		EntityComponent::ComponentTypeID iID = EntityComponent::getComponentTypeID<Components::Input>();
		EntityComponent::ComponentTypeID tagID = EntityComponent::getComponentTypeID<Components::TagClass>();
		//create bitsets
		EntityComponent::ComponentBitMask objMask;
		objMask.set(iID);


		for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				for (Entity ent : it->second)
				{
					Components::Input* input = ecs.getComponent<Components::Input>(ent);
					std::pair<s32, Entity> temp {input->z, ent};
					this->buffer.push_back(temp);
				}
			}
			
		}



		this->buffer.sort(IM_CMP);
		Entity active_hover = 0;

		for (std::pair<s32, Entity> temp : this->buffer)
		{
			f32 camera_x = cam->pos.x;
			f32 camera_y = cam->pos.y;
			f32 zoom = cam->size.y;
			Entity e = temp.second;

			EntityComponent::ComponentTypeID iID = EntityComponent::getComponentTypeID<Components::Input>();
			EntityComponent::ComponentTypeID tID = EntityComponent::getComponentTypeID<Components::Transform>();

			if (!ecs.getBitMask()[e].test(tID)) continue;
			if (!ecs.getBitMask()[e].test(iID)) continue;

			Components::Input* in = ecs.getComponent<Components::Input>(e);

			if(in->on == false) continue;


			Components::Transform* t = ecs.getComponent<Components::Transform>(e);
			if (ecs.getBitMask()[e].test(tagID))
			{
				Components::TagClass* tag = ecs.getComponent<Components::TagClass>(e);
				if (tag->value == Components::Tag::CARDS || tag->value == Components::Tag::UI)
				{
					camera_x = 0;
					camera_y = 0;
					zoom = 1;
				}
			}


			f32 newpos_x = (t->pos_onscreen.x - camera_x) * zoom;
			f32 newpos_y = (t->pos_onscreen.y - camera_y) * zoom;
			f32 newsize_x = t->size_og.x * zoom;
			f32 newsize_y = t->size_og.y * zoom;

			if(cur_drag == e)
			{
				if(AEInputCheckCurr(in->type))
				{
					if (in->onDrag != nullptr) in->onDrag();
					continue;
				}
			}


			if (in->col && point2rect_intersect(newpos_x, newpos_y, newsize_x, newsize_y, f32(this->mousex), f32(this->mousey)))
			{
				if (in->drag == false)
				{

					if (AEInputCheckTriggered(in->type))
					{
						if (in->onClick != nullptr) in->onClick();
					}
					else if(cur_drag != 0 && AEInputCheckReleased(in->type))
					{
						if (in->onClick != nullptr) in->onClick();
						cur_drag = 0;
					}
					else if (active_hover == 0 && in->allow_hover == true)
					{
						if (in->onHover != nullptr)
						{
							in->onHover();
							active_hover = e;
						}
						continue;
					}
				}
				else{
					if ((cur_drag == 0 || cur_drag == e) && AEInputCheckCurr(in->type))
					{
						if(cur_drag == 0)
						{
							if (in->onClick != nullptr) in->onClick();
						}
						if (in->onDrag != nullptr) in->onDrag();
						cur_drag = e;
						
					}
					else if(AEInputCheckReleased(in->type) && cur_drag == e)
					{
						cur_drag = 0;
						//pass for now
					}
					else if(AEInputCheckReleased(in->type) && cur_drag != e)
					{
						if (in->onClick != nullptr) in->onClick();
					}
					else if (active_hover == 0 && in->allow_hover == true)
					{
						if (in->onHover != nullptr)
						{
							in->onHover();
							active_hover = e;
							continue;
						}
					}
				}
			}
			else if(!in->col)
			{
				if (AEInputCheckCurr(in->type))
				{
					if (in->onClick != nullptr) in->onClick();
					active_hover = true;
				}
				else if (AEInputCheckReleased(in->type))
				{
					//pass for now
				}
			}
			else
			{
				if (in->offHover != nullptr && active_hover != e) in->offHover();

			}
		}
			this->buffer.clear();

	}



	bool IM_CMP(std::pair<s32, Entity> const& rhs, std::pair<s32, Entity> const& lhs)
	{
		return (rhs.first > lhs.first) ? true : false;
	}

	InputManager::InputManager()
	{
		this->mousex = 0;
		this->mousey = 0;
		this->cur_drag = 0;
		
	}


}