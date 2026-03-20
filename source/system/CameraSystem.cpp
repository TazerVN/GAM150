#include "pch.h"
#include "CameraSystem.h"

namespace Camera
{

	CameraSystem::CameraSystem() : buffer_x{ 0 }, buffer_y{ 0 }, buffer_zoom{ 1.f }
	{
	}

	void updateCameraPos(Camera::CameraSystem& cam, EntityComponent::Registry& ecs)
	{

		s32 scroll_buffer{ 1 };
		AEInputMouseWheelDelta(&scroll_buffer);

		if (scroll_buffer != 0.0f)
		{
			cam.buffer_zoom += scroll_buffer / (10.f);
			cam.buffer_zoom = AEClamp(cam.buffer_zoom, 0.2f, 3.f);
		}

		Components::Transform* transform = ecs.getComponent<Components::Transform>(cam.id());
		if (AEInputCheckCurr(AEVK_MBUTTON))
		{

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


		transform->size.x = cam.buffer_zoom;
		transform->size.y = cam.buffer_zoom;

	}


	void setOrignalDragPos(Camera::CameraSystem& cam, EntityComponent::Registry& ecs)
	{
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
		else if (AEInputCheckTriggered(AEVK_MBUTTON))
		{
		}

	}

	void CameraSystem::init(EntityComponent::Registry& ecs)
	{
		this->camera_id = ecs.createEntity();
		Components::Transform trans{ {0,0}, {0,0} ,{1.f, 1.f}, {AEGfxGetWindowWidth(), AEGfxGetWindowHeight()},0.0f };
		Components::Input input(AEVK_MBUTTON, true, [this, &ecs]
								{
/*	setOrignalDragPos(*this, ecs);
	updateCameraPos(*this, ecs);*/

								}, nullptr, nullptr, 20, true, false);
		ecs.addComponent(this->camera_id, trans);
		ecs.addComponent(this->camera_id, input);

	}

	Entity CameraSystem::id() const
	{
		return this->camera_id;
	}



	void CameraSystem::update(EntityComponent::Registry& ecs)
	{
		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID tagID = EntityComponent::getComponentTypeID<Components::TagClass>();
		EntityComponent::ComponentBitMask objMask;
		objMask.set(transID);
		objMask.set(tagID);

		Components::Transform* camera = ecs.getComponent<Components::Transform>(this->camera_id);
		setOrignalDragPos(*this, ecs);
		updateCameraPos(*this, ecs);

	}

}