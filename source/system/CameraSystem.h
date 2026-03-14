#pragma once

#include "../types.h"
#include "../ECS/ECSystem.h"
#include <AEEngine.h>



namespace Camera{

	class CameraSystem
	{
		private:
		Entity camera_id{};
		public:
		f32 buffer_x, buffer_y;
		f32 buffer_zoom;
		CameraSystem();
		void init(EntityComponent::Registry& ecs);
		void update(EntityComponent::Registry& ecs);
		Entity id() const;
	};

}