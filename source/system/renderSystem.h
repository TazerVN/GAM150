
#ifndef RS
#define RS

#include "../factory/MeshFactory.h"
#include <vector>
#include "../ECS/ECSystem.h"


namespace RenderSystem
{
	
	class RenderSystem
	{
		private:
		std::vector<Entity> e_buffer{0};
		std::vector<s8> z_buffer{0};
		public:
		RenderSystem();
		void RenderSystem_init(ECS::Registry& ecs);
		void RM_render(ECS::Registry& ecs);

	};
};




#endif // !RS