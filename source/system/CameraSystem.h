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
		AEVec2 mouse;
		CameraSystem();
		void updateCameraPos();
		void init();
		void update();
		Entity id() const;
	};

}