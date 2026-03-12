#pragma once

#include "../ECS/ECSystem.h"

namespace Camera{

	class CameraSystem
	{
		private:
		Entity camera_id{};
		public:
		f32 buffer_x, buffer_y;
		f32 buffer_zoom;
		CameraSystem();
		void init(ECS::Registry& ecs);
		void update(ECS::Registry& ecs);
		Entity id() const;
	};

}