#pragma once

#include "../types.h"
#include "../factory/MeshFactory.h"
#include "../ECS/ECSystem.h"
#include "../util/BST.h"
#include <vector>
#include <utility>




namespace RenderSystem
{
	
	class RenderSystem
	{
		private:

		std::list<std::pair<s32, Entity>> buffer;
		public:
		RenderSystem();
		void RenderSystem_init();
		void RM_render(Entity camera);

	};
};




