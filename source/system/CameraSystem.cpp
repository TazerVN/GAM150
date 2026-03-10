#include "CameraSystem.h"

namespace Camera
{

	void updateCameraPos(Camera::CameraSystem& cam, ECS::Registry& ecs)
	{
		Components::Transform* transform = ecs.getComponent<Components::Transform>(cam.id());

		s32 mousex, mousey;

		AEInputGetCursorPosition(&mousex, &mousey);

		mousex = mousex - f32(AEGfxGetWindowWidth()) * 0.5f;
		mousey = -mousey + f32(AEGfxGetWindowHeight()) * 0.5f;

		mousex = AEClamp(mousex, AEGfxGetWinMinX(), AEGfxGetWinMaxX());
		mousey = AEClamp(mousey, AEGfxGetWinMinY(), AEGfxGetWinMaxY());


		transform->pos.x = cam.buffer_x - f32(mousex);
		transform->pos.y = cam.buffer_y - f32(mousey);


		transform->pos.x = AEClamp(transform->pos.x, AEGfxGetWinMinX(), AEGfxGetWinMaxX());
		transform->pos.y = AEClamp(transform->pos.y, AEGfxGetWinMinY(), AEGfxGetWinMaxY());

	}

	void setOrignalDragPos(Camera::CameraSystem& cam, ECS::Registry& ecs){
		if (AEInputCheckTriggered(AEVK_MBUTTON))
		{
			Components::Transform* transform = ecs.getComponent<Components::Transform>(cam.id());
			s32 mousex, mousey;

			AEInputGetCursorPosition(&mousex, &mousey);

			mousex = mousex - f32(AEGfxGetWindowWidth()) * 0.5f;
			mousey = -mousey + f32(AEGfxGetWindowHeight()) * 0.5f;

			mousex = AEClamp(mousex, AEGfxGetWinMinX(), AEGfxGetWinMaxX());
			mousey = AEClamp(mousey, AEGfxGetWinMinY(), AEGfxGetWinMaxY());

			cam.buffer_x = f32(transform->pos.x) + mousex;
			cam.buffer_y = f32(transform->pos.y) + mousey;

		}
		else if(AEInputCheckTriggered(AEVK_MBUTTON))
		{
		}

	}

	void CameraSystem::init(ECS::Registry& ecs)
	{
		this->camera_id = ecs.createEntity();
		Components::Transform trans{ {0,0}, {0,0} ,{AEGfxGetWindowWidth(), AEGfxGetWindowHeight()}, {AEGfxGetWindowWidth(), AEGfxGetWindowHeight()},0.0f };
		Components::Input input(AEVK_MBUTTON, true, [this, &ecs] { 
			setOrignalDragPos(*this, ecs);
			updateCameraPos(*this, ecs);

								}, nullptr, nullptr, 20, true, false);
		ecs.addComponent(this->camera_id, trans);
		ecs.addComponent(this->camera_id, input);

	}

	Entity CameraSystem::id() const{
		return this->camera_id;
	}



	void CameraSystem::update(ECS::Registry& ecs)
	{
		ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Transform>();
		ECS::ComponentTypeID tagID = ECS::getComponentTypeID<Components::TagClass>();
		ECS::ComponentBitMask objMask;
		objMask.set(transID);
		objMask.set(tagID);

		Components::Transform* camera = ecs.getComponent<Components::Transform>(this->camera_id);


		/*for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				for (Entity ent : it->second)
				{
					if (!ecs.getBitMask()[ent].test(transID)) continue;
					Components::Transform* transform = ecs.getComponent<Components::Transform>(ent);
					Components::TagClass* tag = ecs.getComponent<Components::TagClass>(ent);


					if (tag->value == Components::Tag::CARDS || tag->value == Components::Tag::UI)
					{
						transform->pos_onscreen.x = transform->pos.x + camera->pos.x;
						transform->pos_onscreen.y = transform->pos.y + camera->pos.y;
					}


				}
			}

		}*/
	}

}